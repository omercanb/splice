"""Folds `ord("A")` into a plain int literal when the argument is a single-character
string literal, so it costs nothing at runtime instead of calling ord() on a py::str.
"""

from mypy.nodes import CallExpr, Expression, IntExpr, NameExpr, Node, StrExpr
from mypy.types import Type

from splice.transform.tree_transformer import Transformer, copy_position


def _is_ord_of_char_literal(o: CallExpr) -> bool:
    return (
        isinstance(o.callee, NameExpr)
        and o.callee.fullname == "builtins.ord"
        and len(o.args) == 1
        and isinstance(o.args[0], StrExpr)
        and len(o.args[0].value) == 1
    )


class OrdTransformer(Transformer):
    def __init__(self, types: dict[Expression, Type]):
        super().__init__()
        self.types = types

    def visit_call_expr(self, o: CallExpr) -> Node:
        if _is_ord_of_char_literal(o):
            arg = o.args[0]
            assert isinstance(arg, StrExpr)
            result = copy_position(IntExpr(ord(arg.value)), o)
            if o in self.types:
                self.types[result] = self.types[o]
            return result
        return super().visit_call_expr(o)
