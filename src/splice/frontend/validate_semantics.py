"""
Validation logic for value semantics related checks
Runs after ast transforms for easier validation
"""

from __future__ import annotations

import itertools

from mypy.nodes import CallExpr, Expression, MypyFile
from mypy.types import Type

from splice.analysis.call_graph import is_call_builtin, match_call_arguments
from splice.analysis.mutation import MutationTable
from splice.analysis.structural_aliasing import (
    get_access_path,
    is_access_path_structural_alias,
)
from splice.convert_to_python import convert_to_python
from splice.frontend.validate import Diagnostic, diagnostic
from splice.visitor import Traverser

# Marker used raise a diagnostic to check if positions in the transformed ast match the original positions
_MARKER_FULLNAME = "splice.stdlib._error_after_tree_transform"


class _SemanticsValidator(Traverser):
    def __init__(self, mutations: MutationTable, types: dict[Expression, Type]) -> None:
        self.mutations = mutations
        self.types = types
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
        if not is_call_builtin(o, self.types):
            self.check_exclusivity(o)
        self._enclosing_calls.append(o)
        super().visit_call_expr(o)
        self._enclosing_calls.pop()

    def check_exclusivity(self, o: CallExpr) -> None:
        """Two arguments to one call (self included) can't structurally
        alias if at least one binds to a mutable parameter."""
        claims = []
        for arg_expr, param_var in match_call_arguments(o, self.types):
            path = get_access_path(arg_expr)
            if path is not None:
                claims.append((arg_expr, path, param_var in self.mutations))

        for (expr1, path1, mut1), (expr2, path2, mut2) in itertools.combinations(
            claims, 2
        ):
            if not (mut1 or mut2):
                continue
            if is_access_path_structural_alias(path1, path2) is not None:
                self.report(
                    o,
                    "aliasing-arguments",
                    f"`{convert_to_python(expr1)}` and `{convert_to_python(expr2)}` "
                    "could be the same value, and at least one is mutated here",
                    "pass a copy instead so they can't alias:\n"
                    f"{convert_to_python(expr2)} = copy({convert_to_python(expr2)})",
                )
                return


def validate_semantics(
    tree: MypyFile, mutations: MutationTable, types: dict[Expression, Type]
) -> list[Diagnostic]:
    validator = _SemanticsValidator(mutations, types)
    validator.visit(tree)
    return sorted(validator.diagnostics, key=lambda d: d.position)
