"""Builtin function definitions and utilities for code generation."""

import sys
from dataclasses import dataclass, field
from typing import Optional

from mypy.nodes import Expression, NameExpr, StrExpr


def bool_expr(value: bool) -> NameExpr:
    """A True/False literal carrying the fullname the codegen matches on."""
    node = NameExpr("True" if value else "False")
    node.fullname = "builtins.True" if value else "builtins.False"
    return node


@dataclass(frozen=True)
class BuiltinParam:
    name: str
    default: Optional[Expression] = None
    # Skip this argument entirely rather than filling in a default, useful for sorted()
    omit_if_missing: bool = False


@dataclass(frozen=True)
class BuiltinSignature:
    """A builtin's call shape
    variadic=True means every plain positional argument packs into a tuple (used for print)
    """

    params: list[BuiltinParam] = field(default_factory=list)
    variadic: bool = False


# Builtins whose call shape needs normalizing (a default to fill in, or *args
# to pack). Only these two have one today - everything else in
# SUPPORTED_BUILTIN_FUNCTIONS is plain positional with no optional argument.
BUILTIN_SIGNATURES: dict[str, BuiltinSignature] = {
    "builtins.print": BuiltinSignature(
        variadic=True,
        params=[
            BuiltinParam("sep", StrExpr(" ")),
            BuiltinParam("end", StrExpr("\n")),
        ],
    ),
    # sorted(iterable, *, key=None, reverse=False).
    "builtins.sorted": BuiltinSignature(
        params=[
            BuiltinParam("iterable"),
            BuiltinParam("key", omit_if_missing=True),
            BuiltinParam("reverse", bool_expr(False)),
        ],
    ),
}

# Same idea as BUILTIN_SIGNATURES, but for a builtin method call (l.sort(...))
# rather than a free function - keyed by method name, since a builtin method
# isn't qualified by fullname the way a free function is.
BUILTIN_METHOD_SIGNATURES: dict[str, BuiltinSignature] = {
    # list.sort(*, key=None, reverse=False).
    "sort": BuiltinSignature(
        params=[
            BuiltinParam("key", omit_if_missing=True),
            BuiltinParam("reverse", bool_expr(False)),
        ],
    ),
}


OP_MAP = {
    "is": "__is",
    "is not": "!__is",
    "/": "fdiv",
    "//": "idiv",
    "%": "mod",
    "**": "pow",
}

# Python method names that collide with C++ keywords.
METHOD_RENAMES = {"union": "union_"}

# Builtins whose bare C++ spelling collides with a C library function visible
# at global scope. `round(2.5)` would match both ::round(double) and ours,
# since _float is double, so the call is qualified to settle it.
QUALIFIED_BUILTINS = {"builtins.round": "py::round"}

SCALAR_CONSTRUCTORS = {
    "int": "to_int",
    "float": "to_float",
    "bool": "to_bool",
    "str": "to_str",
    "int8": "int8_t",
    "uint8": "uint8_t",
    "int16": "int16_t",
    "uint16": "uint16_t",
    "int32": "int32_t",
    "uint32": "uint32_t",
    "int64": "int64_t",
    "uint64": "uint64_t",
}

# A constructor call with no arguments (list(), set()) gives C++ template
# argument deduction nothing to work with, so these need their element type
# spelled out explicitly rather than left to CTAD.
EXPLICIT_TYPE_CONSTRUCTORS = {"list", "dict", "set"}

FIXED_WIDTH_INT_TYPES = {
    "splice.stdlib.int8": "int8_t",
    "splice.stdlib.uint8": "uint8_t",
    "splice.stdlib.int16": "int16_t",
    "splice.stdlib.uint16": "uint16_t",
    "splice.stdlib.int32": "int32_t",
    "splice.stdlib.uint32": "uint32_t",
    "splice.stdlib.int64": "int64_t",
    "splice.stdlib.uint64": "uint64_t",
}

# Scalars are numbers and bools (so copying one is free)
# We do various special cases for these like not needing copy functions
SCALAR_FULLNAMES = {
    "builtins.int",
    "builtins.float",
    "builtins.bool",
    *FIXED_WIDTH_INT_TYPES,
}

# Exceptions catchable by an except clause (runtime/exceptions.h). Flat
# hierarchy, so a base like LookupError has no equivalent and is rejected
# rather than mapped to PyException, which would over-catch.
EXCEPTION_TYPES = {
    "builtins.BaseException": "PyException",
    "builtins.Exception": "PyException",
    "builtins.IndexError": "IndexError",
    "builtins.KeyError": "KeyError",
    "builtins.ValueError": "ValueError",
    "builtins.TypeError": "TypeError",
    "builtins.FileNotFoundError": "FileNotFoundError",
    "builtins.AssertionError": "AssertionError",
}


BOOL_OP_MACROS = {"and": "_and", "or": "_or"}

# Real Python builtins that the runtime actually implements (see runtime/*.h).
# Anything not listed here has no C++ equivalent yet.
SUPPORTED_BUILTIN_FUNCTIONS = {
    "builtins.print",
    "builtins.len",
    "builtins.range",
    "builtins.enumerate",
    "builtins.zip",
    "builtins.map",
    "builtins.filter",
    "builtins.sorted",
    "builtins.reversed",
    "builtins.sum",
    "builtins.any",
    "builtins.all",
    "builtins.min",
    "builtins.max",
    "builtins.divmod",
    "builtins.round",
    "builtins.ord",
    "builtins.chr",
    "builtins.repr",
    "builtins.hash",
    "builtins.open",
    "builtins.int",
    "builtins.float",
    "builtins.bool",
    "builtins.str",
    "builtins.list",
    "builtins.dict",
    "builtins.set",
    "builtins.bytes",
    # Raising/catching one of these is handled separately, by
    # check_exception_class - just don't double-flag the constructor call.
    *EXCEPTION_TYPES,
}

# Used to differentiate imports from modules that Python comes with builtin vs imports from .pyi files the user defines
KNOWN_STDLIB_MODULES = sys.stdlib_module_names

# Individual Python stdlib functions to allow through as they get implemented.
SUPPORTED_STDLIB_FUNCTIONS: set[str] = set()
