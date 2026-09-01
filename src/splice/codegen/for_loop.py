"""Translating common python for loop cases efficiently"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import TYPE_CHECKING, Callable

from mypy.nodes import CallExpr
from mypy.nodes import Expression as MypyExpression
from mypy.nodes import ForStmt, NameExpr

from splice.ast_utils import get_int_value
from splice.codegen.typegen import cpp_type
from splice.namer import temp_name

if TYPE_CHECKING:
    from splice.codegen.statement_codegen import StatementCodegen


@dataclass
class LoopHeader:
    """A C++ for statement, plus any lines that have to open its body.

    A for statement and a comprehension generator walk their iterable the same
    way and differ only in what goes inside, so the translators below build one
    of these instead of writing a body themselves.
    """

    line: str
    opening: list[str] = field(default_factory=list)


def translate_for_stmt(codegen: StatementCodegen, for_stmt: ForStmt) -> None:
    """Translate a MyPy ForStmt to C++ code."""
    header = loop_header(codegen, for_stmt.index, for_stmt.expr)
    write_loop(codegen, header, lambda: codegen.visit_statements(for_stmt.body.body))


def write_loop(
    codegen: StatementCodegen, header: LoopHeader, write_body: Callable[[], None]
) -> None:
    """Open a loop, let write_body fill it in, and close it."""
    codegen.emit(f"{header.line} {{")
    codegen.indent()
    for line in header.opening:
        codegen.emit(line)
    write_body()
    codegen.unindent()
    codegen.emit("}")


def loop_header(
    codegen: StatementCodegen, index: MypyExpression, iterable: MypyExpression
) -> LoopHeader:
    """The C++ loop that walks `iterable`, binding each item to `index`."""
    # Match on the iterator expression type
    if isinstance(iterable, CallExpr) and isinstance(iterable.callee, NameExpr):
        func_name = iterable.callee.name
        args = iterable.args

        # for i in range(len(list))
        if (
            func_name == "range"
            and len(args) == 1
            and isinstance(args[0], CallExpr)
            and isinstance(args[0].callee, NameExpr)
            and args[0].callee.name == "len"
        ):
            return for_range_len(codegen, index, args[0].args[0])

        # for i in range(...)
        if func_name == "range":
            return for_range(codegen, index, args)

    # for i in iterable (generic)
    return for_generic(codegen, index, iterable)


def hoist_variable_declaration(
    codegen: StatementCodegen, value: str, cpp_type: str, variable_name_prefix: str
) -> str:
    """Evaluate value into a temporary before the loop and return the name of the temporary."""
    # Used because python evaluates arguments to range/len once before iterating,
    # while a C++ for loop reevaluates every iteration.
    temp = temp_name(variable_name_prefix)
    codegen.emit(f"{cpp_type} {temp} = {value};")
    return temp


def for_range_len(
    codegen: StatementCodegen, index: MypyExpression, inner_iterable: MypyExpression
) -> LoopHeader:
    """Translate for i in range(len(list))"""
    assert isinstance(index, NameExpr)
    target = index.name
    inner_iter_expr = codegen.get_expr(inner_iterable)
    stop = hoist_variable_declaration(
        codegen, f"len({inner_iter_expr})", "int64_t", "len"
    )
    return LoopHeader(f"for ({target} = 0; {target} < {stop}; ++{target})")


def for_range(
    codegen: StatementCodegen, index: MypyExpression, args: list[MypyExpression]
) -> LoopHeader:
    """Translate: for i in range(stop) or range(start, stop) or range(start, stop, step)"""
    if len(args) in (1, 2):
        return for_range_no_step(codegen, index, args)

    step = get_int_value(args[2], codegen.types)
    if step is not None:
        return for_range_constant_step(codegen, index, args, step)
    return for_range_unknown_step(codegen, index, args)


def for_range_no_step(
    codegen: StatementCodegen, index: MypyExpression, args: list[MypyExpression]
) -> LoopHeader:
    """Translate: for i in range(stop) or range(start, stop)"""
    assert isinstance(index, NameExpr)
    target = index.name

    if len(args) == 1:
        stop = hoist_variable_declaration(
            codegen, codegen.get_expr(args[0]), cpp_type(codegen.types[args[0]]), "stop"
        )
        return LoopHeader(f"for ({target} = 0; {target} < {stop}; ++{target})")

    start = codegen.get_expr(args[0])
    stop = hoist_variable_declaration(
        codegen, codegen.get_expr(args[1]), cpp_type(codegen.types[args[1]]), "stop"
    )
    return LoopHeader(f"for ({target} = {start}; {target} < {stop}; ++{target})")


def for_range_constant_step(
    codegen: StatementCodegen,
    index: MypyExpression,
    args: list[MypyExpression],
    step: int,
) -> LoopHeader:
    """Translate: for i in range(start, stop, constant_step)"""
    assert isinstance(index, NameExpr)
    target = index.name
    start = codegen.get_expr(args[0])
    stop = hoist_variable_declaration(
        codegen, codegen.get_expr(args[1]), cpp_type(codegen.types[args[1]]), "stop"
    )
    comparison = "<" if step > 0 else ">"
    return LoopHeader(
        f"for ({target} = {start}; {target} {comparison} {stop}; "
        f"{target} += {step})"
    )


def for_range_unknown_step(
    codegen: StatementCodegen, index: MypyExpression, args: list[MypyExpression]
) -> LoopHeader:
    """Translate: for i in range(start, stop, dynamic_step)"""
    assert isinstance(index, NameExpr)
    target = index.name
    start = codegen.get_expr(args[0])
    stop = hoist_variable_declaration(
        codegen, codegen.get_expr(args[1]), cpp_type(codegen.types[args[1]]), "stop"
    )
    step = hoist_variable_declaration(
        codegen, codegen.get_expr(args[2]), cpp_type(codegen.types[args[2]]), "step"
    )

    return LoopHeader(
        f"for ({target} = {start};; {target} += {step})",
        [
            f"if (({step} > 0 && {target} >= {stop}) || "
            f"({step} < 0 && {target} <= {stop})) break;"
        ],
    )


def for_generic(
    codegen: StatementCodegen, index: MypyExpression, iterable: MypyExpression
) -> LoopHeader:
    """Translate: for var in iterable (standard C++ iterator protocol)"""
    target = codegen.get_expr(index, lvalue=True)
    # `for x in [1, 2, 3]:` iterates a temporary; the containers' iterators
    # hold only a reference, which would dangle once this statement ends.
    # Binding it to auto&& first extends its lifetime for the whole loop.
    range_var = hoist_variable_declaration(
        codegen, codegen.get_expr(iterable), "auto &&", "range"
    )
    item_var = temp_name("item")

    return LoopHeader(
        f"for (auto &&{item_var} : {range_var})",
        [f"{target} = {item_var};"],
    )
