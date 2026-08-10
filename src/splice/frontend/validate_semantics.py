"""
Validation logic for value semantics related checks
Runs after ast transforms for easier validation
"""

from __future__ import annotations

from mypy.nodes import CallExpr, MypyFile

from splice.frontend.validate import Diagnostic, diagnostic
from splice.visitor import Traverser

_MARKER_FULLNAME = "splice.stdlib._error_after_tree_transform"


class _SemanticsValidator(Traverser):
    def __init__(self) -> None:
        self.diagnostics: list[Diagnostic] = []
        self._enclosing_calls: list[CallExpr] = []

    def visit_call_expr(self, o: CallExpr) -> None:
        if getattr(o.callee, "fullname", None) == _MARKER_FULLNAME:
            target = self._enclosing_calls[-1] if self._enclosing_calls else o
            self.diagnostics.append(
                diagnostic(
                    target,
                    "error-after-tree-transform",
                    "position-check marker found here",
                    "test marker - remove once the position check has run",
                )
            )
            return
        self._enclosing_calls.append(o)
        super().visit_call_expr(o)
        self._enclosing_calls.pop()


def validate_semantics(tree: MypyFile) -> list[Diagnostic]:
    validator = _SemanticsValidator()
    validator.visit(tree)
    return sorted(validator.diagnostics, key=lambda d: d.position)
