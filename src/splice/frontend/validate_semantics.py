"""
Validation logic for value semantics related checks
Runs after ast transforms for easier validation
"""

from __future__ import annotations

import itertools

from mypy.nodes import (
    AssignmentStmt,
    CallExpr,
    Context,
    Expression,
    ForStmt,
    MemberExpr,
    MypyFile,
    NameExpr,
    OperatorAssignmentStmt,
    ReturnStmt,
)
from mypy.types import Type

from splice.analysis.builtin_effects import builtin_operation_effect, type_name
from splice.analysis.call_graph import (
    is_call_builtin,
    is_call_splice_intrinsic,
    match_call_arguments,
)
from splice.analysis.mutation import MutationTable
from splice.analysis.structural_aliasing import (
    AccessPath,
    get_access_path,
    get_loop_container_paths,
    is_access_path_structural_alias,
)
from splice.ast_utils import replace_in_source, source_text
from splice.frontend.validate import Diagnostic, diagnostic
from splice.visitor import Traverser

# Marker used raise a diagnostic to check if positions in the transformed ast match the original positions
_MARKER_FULLNAME = "splice.stdlib._error_after_tree_transform"
_COPY_FULLNAME = "splice.stdlib.copy"
# A scalar is a plain C++ value with no indirection - copying one is free
# and there's nothing for it to alias, so copy() adds nothing here.
_SCALAR_TYPES = {
    "int",
    "float",
    "bool",
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64",
    "uint64",
}


def _is_copy_call(expr: Expression) -> bool:
    return (
        isinstance(expr, CallExpr)
        and isinstance(expr.callee, NameExpr)
        and expr.callee.fullname == _COPY_FULLNAME
    )


class _SemanticsValidator(Traverser):
    def __init__(
        self, mutations: MutationTable, types: dict[Expression, Type], source: str
    ) -> None:
        self.mutations = mutations
        self.types = types
        self.source = source
        self.diagnostics: list[Diagnostic] = []
        self._enclosing_calls: list[CallExpr] = []
        # Containers currently being iterated by an enclosing for loop, each
        # paired with that loop's full iterated expression (for building a
        # hint) - a stack, since loops can nest.
        self._loop_claims: list[tuple[Expression, AccessPath, Expression]] = []

    def report(self, node, kind: str, message: str, hint: str) -> None:
        self.diagnostics.append(diagnostic(node, kind, message, hint))

    def visit_call_expr(self, o: CallExpr) -> None:
        if getattr(o.callee, "fullname", None) == _MARKER_FULLNAME:
            target = self._enclosing_calls[-1] if self._enclosing_calls else o
            self.report(
                target,
                "error-after-tree-transform",
                "position-check marker found here",
                "test marker - remove once the position check has run",
            )
            return
        if is_call_splice_intrinsic(o):
            pass
        elif is_call_builtin(o, self.types):
            self.check_builtin_exclusivity(o)
        else:
            self.check_exclusivity(o)
        self._enclosing_calls.append(o)
        super().visit_call_expr(o)
        self._enclosing_calls.pop()

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> None:
        self.check_compound_assign_exclusivity(o)
        super().visit_operator_assignment_stmt(o)

    def visit_assignment_stmt(self, o: AssignmentStmt) -> None:
        if len(o.lvalues) == 1:
            self.check_hand_off_copy(o, o.rvalue, is_return=False)
        super().visit_assignment_stmt(o)

    def visit_return_stmt(self, o: ReturnStmt) -> None:
        if o.expr is not None:
            self.check_hand_off_copy(o, o.expr, is_return=True)
        super().visit_return_stmt(o)

    def check_hand_off_copy(
        self, o: Context, source: Expression, is_return: bool
    ) -> None:
        """An assignment or return of an existing name, field, or subscript
        needs an explicit copy() - without it, the target would end up
        aliasing the same value instead of getting its own.

        Returning a plain local is exempt: the name dies at the return, so
        nothing left in this function can mutate it. A parameter or self is
        not exempt even on return, since the caller keeps its own live
        reference to that storage.
        """
        if _is_copy_call(source):  # copy() already used
            return
        path = get_access_path(source)
        if path is None:  # Not an lvalue
            return
        if type_name(self.types.get(source)) in _SCALAR_TYPES:
            # A scalar type is exempt
            return
        if is_return and not path.root.is_argument:
            # Returning a local
            return
        text = source_text(source, self.source)
        self.report(
            o,
            "missing-copy",
            f"`{text}` needs an explicit copy here",
            f"wrap it in copy():\ncopy({text})",
        )

    def visit_for_stmt(self, o: ForStmt) -> None:
        self.visit(o.index)
        self.visit(o.expr)
        containers = get_loop_container_paths(o.expr)
        self._loop_claims.extend((expr, path, o.expr) for expr, path in containers)
        self.visit(o.body)
        del self._loop_claims[len(self._loop_claims) - len(containers) :]
        if o.else_body is not None:
            self.visit(o.else_body)

    def check_compound_assign_exclusivity(self, o: OperatorAssignmentStmt) -> None:
        """A compound assignment always mutates its target, so `l += l`
        can't structurally alias itself."""
        claims: list[tuple[Expression, AccessPath, bool]] = []
        lhs_path = get_access_path(o.lvalue)
        if lhs_path is not None:
            claims.append((o.lvalue, lhs_path, True))
            self._check_loop_claims(o.lvalue, lhs_path)
        rhs_path = get_access_path(o.rvalue)
        if rhs_path is not None:
            claims.append((o.rvalue, rhs_path, False))
        self._check_claims(o, claims)

    def check_exclusivity(self, o: CallExpr) -> None:
        """Two arguments to one call (self included) can't structurally
        alias if at least one binds to a mutable parameter."""
        claims: list[tuple[Expression, AccessPath, bool]] = []
        for arg_expr, param_var in match_call_arguments(o, self.types):
            path = get_access_path(arg_expr)
            if path is not None:
                mutable = param_var in self.mutations
                claims.append((arg_expr, path, mutable))
                if mutable:
                    self._check_loop_claims(arg_expr, path)
        self._check_claims(o, claims)

    def check_builtin_exclusivity(self, o: CallExpr) -> None:
        """A builtin method call (eg. l.append(x)) has no FuncDef to read
        parameters from, so mutability comes from the builtin effects table
        instead, keyed by argument index with self at index 0."""
        if not isinstance(o.callee, MemberExpr):
            return
        effect = builtin_operation_effect(self.types[o.callee.expr], o.callee.name)
        if effect is None or not effect.mutated_args:
            return

        claims: list[tuple[Expression, AccessPath, bool]] = []
        for i, arg_expr in enumerate([o.callee.expr, *o.args]):
            path = get_access_path(arg_expr)
            if path is not None:
                mutable = i in effect.mutated_args
                claims.append((arg_expr, path, mutable))
                if mutable:
                    self._check_loop_claims(arg_expr, path)
        self._check_claims(o, claims)

    def _check_loop_claims(
        self, mutated_expr: Expression, mutated_path: AccessPath
    ) -> None:
        """A container being iterated by an enclosing for loop can't alias
        anything mutated inside that loop's own body."""
        for container_expr, container_path, loop_iterable in self._loop_claims:
            if (
                is_access_path_structural_alias(mutated_path, container_path)
                is not None
            ):
                mutated_text = source_text(mutated_expr, self.source)
                container_text = source_text(container_expr, self.source)
                rewritten = replace_in_source(
                    loop_iterable,
                    container_expr,
                    f"copy({container_text})",
                    self.source,
                )
                if rewritten is not None:
                    hint = f"iterate a copy so mutating it inside the loop can't alias:\n{rewritten}"
                else:
                    hint = f"iterate a copy so mutating it inside the loop can't alias, eg. wrap `{container_text}` in copy(...)"
                self.report(
                    mutated_expr,
                    "aliasing-loop-container",
                    f"`{mutated_text}` could alias `{container_text}`, which this loop is iterating",
                    hint,
                )
                return

    def _check_claims(
        self, o: Context, claims: list[tuple[Expression, AccessPath, bool]]
    ) -> None:
        for (expr1, path1, mut1), (expr2, path2, mut2) in itertools.combinations(
            claims, 2
        ):
            if not (mut1 or mut2):
                continue
            if is_access_path_structural_alias(path1, path2) is not None:
                text1 = source_text(expr1, self.source)
                text2 = source_text(expr2, self.source)
                rewritten = replace_in_source(o, expr2, f"copy({text2})", self.source)
                if rewritten is not None:
                    hint = f"pass a copy so they can't alias:\n{rewritten}"
                else:
                    hint = f"pass a copy so they can't alias, eg. wrap `{text2}` in copy(...)"
                self.report(
                    o,
                    "aliasing-arguments",
                    f"`{text1}` and `{text2}` could alias, and aliasing is not permitted for mutable parameters",
                    hint,
                )
                return


def validate_semantics(
    tree: MypyFile, mutations: MutationTable, types: dict[Expression, Type], source: str
) -> list[Diagnostic]:
    validator = _SemanticsValidator(mutations, types, source)
    validator.visit(tree)
    return sorted(validator.diagnostics, key=lambda d: d.position)
