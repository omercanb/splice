"""Raise and try statement translation from MyPy AST to C++."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mypy.nodes import CallExpr
from mypy.nodes import Expression as MypyExpression
from mypy.nodes import NameExpr, RaiseStmt, TryStmt, TypeInfo

from splice.codegen.builtins import EXCEPTION_TYPES

if TYPE_CHECKING:
    from splice.codegen.statement_codegen import StatementCodegen


def exception_name(expression: MypyExpression) -> str:
    """The C++ name of a directly named Python exception class."""
    assert isinstance(expression, NameExpr)
    return EXCEPTION_TYPES[expression.fullname]


def exception_type(type_expression: MypyExpression | None) -> str:
    """The C++ type a handler catches. A bare `except:` catches the base."""
    if type_expression is None:
        return EXCEPTION_TYPES["builtins.BaseException"]
    return exception_name(type_expression)


def names_a_class(expression: MypyExpression) -> bool:
    """Whether `raise X` names an exception class rather than an instance."""
    return isinstance(expression, NameExpr) and isinstance(expression.node, TypeInfo)


def translate_raise_stmt(codegen: StatementCodegen, raise_stmt: RaiseStmt) -> None:
    """Translate raise."""
    expression = raise_stmt.expr
    if expression is None:
        # A bare raise re-raises whatever is being handled, exactly like throw.
        codegen.emit("throw;")
        return
    if names_a_class(expression):
        codegen.emit(f'throw {exception_name(expression)}("");')
        return
    if isinstance(expression, CallExpr) and names_a_class(expression.callee):
        name = exception_name(expression.callee)
        message = codegen.get_expr(expression.args[0]) if expression.args else '""'
        codegen.emit(f"throw {name}({message});")
        return
    # An exception that already exists. throw would slice it to its declared
    # type, so go through the virtual raise().
    codegen.emit(f"{codegen.get_expr(expression)}.raise();")


def translate_try_stmt(codegen: StatementCodegen, try_stmt: TryStmt) -> None:
    """Translate try/except/else/finally.

    finally becomes a Finally guard so it also runs on return, break and
    continue. else needs a flag because a handler falls through, and sits
    outside the try so the handlers do not catch what it raises.
    """
    # The guard and the flag belong to this statement alone, so they get a
    # block of their own. That also keeps their names clear of the ones a
    # sibling try emits into the same scope.
    scoped = try_stmt.finally_body is not None or try_stmt.else_body is not None
    if scoped:
        codegen.emit("{")
        codegen.indent()

    if try_stmt.finally_body is not None:
        codegen.emit("Finally __finally([&] {")
        codegen.visit_block(try_stmt.finally_body)
        codegen.emit("});")

    if try_stmt.else_body is not None:
        codegen.emit("bool __thrown = false;")

    write_handlers(codegen, try_stmt)

    if try_stmt.else_body is not None:
        codegen.emit("if (!__thrown) {")
        codegen.visit_block(try_stmt.else_body)
        codegen.emit("}")

    if scoped:
        codegen.unindent()
        codegen.emit("}")


def write_handlers(codegen: StatementCodegen, try_stmt: TryStmt) -> None:
    """The try body and its catch clauses."""
    if not try_stmt.handlers:
        # A try/finally has nothing to catch, and a catch-less try is not valid
        # C++. The guard alone does the job.
        codegen.visit_statements(try_stmt.body.body)
        return

    codegen.emit("try {")
    codegen.visit_block(try_stmt.body)
    for type_expression, variable, handler in zip(
        try_stmt.types, try_stmt.vars, try_stmt.handlers
    ):
        # The catch clause declares the handler variable, so unlike other
        # locals it is not hoisted.
        declaration = f"&{variable.name}" if variable is not None else "&"
        codegen.emit(f"}} catch ({exception_type(type_expression)} {declaration}) {{")
        if try_stmt.else_body is not None:
            codegen.indent()
            codegen.emit("__thrown = true;")
            codegen.unindent()
        codegen.visit_block(handler)
    codegen.emit("}")
