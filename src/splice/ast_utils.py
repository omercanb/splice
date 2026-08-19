"""Small stand-alone helpers for working with mypy AST nodes."""

from mypy.nodes import Context, Expression, IntExpr, UnaryExpr
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


def source_text(node: Context, source: str) -> str:
    """The original source text a node's position covers.

    Use this instead of convert_to_python for a node that may have been
    changed by an AST transform - convert_to_python rebuilds text from the
    node itself, so it would print the transformed code (eg. `a.__getitem__(0)`
    instead of `a[0]`). A transform keeps the position pointing at the
    original text even when it changes the node, so reading the source
    directly gives back what the user actually wrote.
    """
    lines = source.splitlines()
    end_line = node.end_line if node.end_line is not None else node.line
    end_column = node.end_column if node.end_column is not None else node.column
    if node.line == end_line:
        return lines[node.line - 1][node.column : end_column]
    first = lines[node.line - 1][node.column :]
    middle = lines[node.line : end_line - 1]
    last = lines[end_line - 1][:end_column]
    return "\n".join([first, *middle, last])


def replace_in_source(outer: Context, target: Context, replacement: str, source: str) -> str | None:
    """outer's source text, with target's span swapped for `replacement`.

    Only handles outer and target both sitting on one line - the only case
    this is used for so far (a call and one of its own arguments). Returns
    None rather than wrong text when that doesn't hold.
    """
    target_end_line = target.end_line if target.end_line is not None else target.line
    if outer.line != target.line or target.line != target_end_line:
        return None
    outer_text = source_text(outer, source)
    target_end_column = target.end_column if target.end_column is not None else target.column
    start = target.column - outer.column
    end = target_end_column - outer.column
    return outer_text[:start] + replacement + outer_text[end:]
