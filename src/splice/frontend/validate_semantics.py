"""
Validation logic for value semantics related checks
Runs after ast transforms for easier validation
"""

from __future__ import annotations

import itertools

from mypy.nodes import CallExpr, Expression, MemberExpr, MypyFile
from mypy.types import Type

from splice.analysis.builtin_effects import builtin_operation_effect
from splice.analysis.call_graph import is_call_builtin, match_call_arguments
from splice.analysis.mutation import MutationTable
from splice.analysis.structural_aliasing import (
    AccessPath,
    get_access_path,
    is_access_path_structural_alias,
)
from splice.ast_utils import replace_in_source, source_text
from splice.frontend.validate import Diagnostic, diagnostic
from splice.visitor import Traverser

# Marker used raise a diagnostic to check if positions in the transformed ast match the original positions
_MARKER_FULLNAME = "splice.stdlib._error_after_tree_transform"


class _SemanticsValidator(Traverser):
    def __init__(
        self, mutations: MutationTable, types: dict[Expression, Type], source: str
    ) -> None:
        self.mutations = mutations
        self.types = types
        self.source = source
        self.diagnostics: list[Diagnostic] = []
        self._enclosing_calls: list[CallExpr] = []

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
        if is_call_builtin(o, self.types):
            self.check_builtin_exclusivity(o)
        else:
            self.check_exclusivity(o)
        self._enclosing_calls.append(o)
        super().visit_call_expr(o)
        self._enclosing_calls.pop()

    def check_exclusivity(self, o: CallExpr) -> None:
        """Two arguments to one call (self included) can't structurally
        alias if at least one binds to a mutable parameter."""
        claims: list[tuple[Expression, AccessPath, bool]] = []
        for arg_expr, param_var in match_call_arguments(o, self.types):
            path = get_access_path(arg_expr)
            if path is not None:
                claims.append((arg_expr, path, param_var in self.mutations))
        self._check_claims(o, claims)

    def check_builtin_exclusivity(self, o: CallExpr) -> None:
        """Same check as check_exclusivity, but for a builtin method call
        (eg. l.append(x)), where there's no FuncDef to read parameters from.
        """
        if not isinstance(o.callee, MemberExpr):
            return
        effect = builtin_operation_effect(self.types[o.callee.expr], o.callee.name)
        if effect is None or not effect.mutated_args:
            return

        claims: list[tuple[Expression, AccessPath, bool]] = []
        for i, arg_expr in enumerate([o.callee.expr, *o.args]):
            path = get_access_path(arg_expr)
            if path is not None:
                claims.append((arg_expr, path, i in effect.mutated_args))
        self._check_claims(o, claims)

    def _check_claims(
        self, o: CallExpr, claims: list[tuple[Expression, AccessPath, bool]]
    ) -> None:
        for (expr1, path1, mut1), (expr2, path2, mut2) in itertools.combinations(
            claims, 2
        ):
            if not (mut1 or mut2):
                continue
            if is_access_path_structural_alias(path1, path2) is not None:
                text1 = source_text(expr1, self.source)
                text2 = source_text(expr2, self.source)
                rewritten = replace_in_source(
                    o, expr2, f"copy({text2})", self.source
                )
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
