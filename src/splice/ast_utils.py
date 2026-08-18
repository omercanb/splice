"""Small stand-alone helpers for working with mypy AST nodes."""

from mypy.nodes import Expression, IntExpr, OpExpr


def get_int_literal(expr: Expression) -> int | None:
    """Extract a compile-time int from a literal, incl. unary +/-."""
    if isinstance(expr, IntExpr):
        return expr.value
    if isinstance(expr, OpExpr):
        # Handle unary minus: -5
        if expr.op == "-" and isinstance(expr.left, IntExpr):
            return -expr.left.value
        if expr.op == "+" and isinstance(expr.left, IntExpr):
            return expr.left.value
    return None
