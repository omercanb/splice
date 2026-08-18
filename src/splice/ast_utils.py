"""Small stand-alone helpers for working with mypy AST nodes."""

from mypy.nodes import Expression, IntExpr, UnaryExpr
from mypy.types import Type

type TypeTable = dict[Expression, Type]


def get_int_literal(expr: Expression) -> int | None:
    """Extract a compile-time int from a literal, incl. unary +/-.

    `-1` parses as UnaryExpr(op="-", expr=IntExpr(1)), not a negative
    IntExpr or a binary OpExpr.
    """
    if isinstance(expr, IntExpr):
        return expr.value
    if isinstance(expr, UnaryExpr) and isinstance(expr.expr, IntExpr):
        if expr.op == "-":
            return -expr.expr.value
        if expr.op == "+":
            return expr.expr.value
    return None
