"""Small stand-alone helpers for working with mypy AST nodes."""

from mypy.nodes import Context, Expression, IntExpr, UnaryExpr
from mypy.types import Instance, LiteralType, Type, get_proper_type

type TypeTable = dict[Expression, Type]


def get_int_value(expr: Expression, types: TypeTable) -> int | None:
    """Return a value of an integer if its available at compile time"""
    n = get_int_literal(expr)
    if n is not None:
        return n
    if expr in types:
        n = literal_int_value(types[expr])
        if n is not None:
            return n
    return None


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


def literal_int_value(t: Type) -> int | None:
    """A compile time integer. Stored as a type in mypys type system"""
    # This can be a Literal[N] or a Final variable
    proper = get_proper_type(t)
    if isinstance(proper, LiteralType) and isinstance(
        proper.value, int
    ):  # A final or literal[n]
        return proper.value
    if (
        isinstance(proper, Instance)
        and proper.last_known_value is not None
        and isinstance(proper.last_known_value.value, int)
    ):  # An int expressions
        return proper.last_known_value.value
    return None


def source_text(node: Context, source: str) -> str:
    """The original source text a node's position covers."""
    lines = source.splitlines()
    end_line = node.end_line if node.end_line is not None else node.line
    end_column = node.end_column if node.end_column is not None else node.column
    if node.line == end_line:
        return lines[node.line - 1][node.column : end_column]
    first = lines[node.line - 1][node.column :]
    middle = lines[node.line : end_line - 1]
    last = lines[end_line - 1][:end_column]
    return "\n".join([first, *middle, last])


def replace_in_source(
    outer: Context, target: Context, replacement: str, source: str
) -> str | None:
    """outer's source text, with target's span swapped for `replacement`.

    target must be a sub-expression of outer, both on the same line. Returns
    None instead of wrong text when that doesn't hold.
    """
    target_end_line = target.end_line if target.end_line is not None else target.line
    if outer.line != target.line or target.line != target_end_line:
        return None
    outer_text = source_text(outer, source)
    target_end_column = (
        target.end_column if target.end_column is not None else target.column
    )
    start = target.column - outer.column
    end = target_end_column - outer.column
    return outer_text[:start] + replacement + outer_text[end:]
