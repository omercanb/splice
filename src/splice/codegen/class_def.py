"""Class translation from MyPy AST to C++.

A class becomes a plain C++ class, used by value like the other containers,
and its methods stay methods. Nothing special happens for the dunders: the
runtime already dispatches to __str__, __len__ and __bool__ by name, so a
user's class satisfies those protocols the same way list and dict do.

The C++ constructor does no work of its own. It forwards to the __init__ the
user wrote, which is emitted as an ordinary method alongside the rest.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from mypy.nodes import ClassDef, Decorator, FuncDef, Var
from mypy.types import Type

from splice.codegen.translation_utils import (
    translate_func_signature,
    translate_parameters,
)

if TYPE_CHECKING:
    from splice.codegen.statement_codegen import StatementCodegen

INIT = "__init__"


def write_class_declaration(codegen: StatementCodegen, class_def: ClassDef) -> None:
    """Emit a classes full structure, defining methods as signatures only"""
    codegen.current_line = class_def.line
    codegen.emit(f"class {class_def.name} {{")
    codegen.emit("  public:")
    codegen.indent()

    write_attributes(codegen, class_def)
    write_constructor(codegen, class_def)
    for method in methods(class_def):
        signature = translate_func_signature(
            method,
            codegen.expr_codegen,
            mutations=codegen.mutations,
            flatten=method in codegen.hotpath_funcs,
        )
        codegen.current_line = method.line
        codegen.emit(f"{signature};")

    codegen.unindent()
    codegen.emit("};")
    codegen.emit("")


def write_class_bodies(codegen: StatementCodegen, class_def: ClassDef) -> None:
    """Emit all methods in the class out of line"""
    for method in methods(class_def):
        header = translate_func_signature(
            method,
            codegen.expr_codegen,
            mutations=codegen.mutations,
            qualifier=f"{class_def.name}::",
            flatten=method in codegen.hotpath_funcs,
            include_defaults=False,
        )
        codegen.emit_function_body(f"{header} {{", method)


def attributes(class_def: ClassDef) -> list[tuple[str, Type]]:
    """The instance attributes, annotated in the body or assigned in __init__.

    mypy has already inferred the assigned ones, so both arrive here alike.
    """
    return [
        (name, symbol.type)
        for name, symbol in class_def.info.names.items()
        if isinstance(symbol.node, Var) and symbol.type is not None
    ]


def methods(class_def: ClassDef) -> list[FuncDef]:
    """The methods, in the order they were written. A decorator's identity is dropped, except @hotpath's effect on the signature (StatementCodegen.hotpath_funcs)."""
    return [
        statement.func if isinstance(statement, Decorator) else statement
        for statement in class_def.defs.body
        if isinstance(statement, (FuncDef, Decorator))
    ]


def write_attributes(codegen: StatementCodegen, class_def: ClassDef) -> None:
    declared = attributes(class_def)
    for name, attribute_type in declared:
        print(name, attribute_type)
        print(codegen.translate_declaration(name, attribute_type))
        codegen.emit(codegen.translate_declaration(name, attribute_type))
    if declared:
        codegen.emit("")


def write_constructor(codegen: StatementCodegen, class_def: ClassDef) -> None:
    """A constructor that only forwards to __init__."""
    init = next((method for method in methods(class_def) if method.name == INIT), None)
    if init is None:
        return

    own_arguments = [
        argument for argument in init.arguments if not argument.variable.is_self
    ]
    parameters = translate_parameters(init, codegen.expr_codegen, codegen.mutations)
    arguments = [argument.variable.name for argument in own_arguments]
    codegen.emit(
        f"{class_def.name}({', '.join(parameters)}) "
        f"{{ {INIT}({', '.join(arguments)}); }}"
    )
    codegen.emit("")

    # Every local gets pre-declared as a bare `ClassName name;` ahead of its
    # first assignment (see get_declarations), so the type has to be
    # default-constructible even when __init__ requires arguments.
    if not all(argument.initializer is not None for argument in own_arguments):
        codegen.emit(f"{class_def.name}() = default;")
        codegen.emit("")
