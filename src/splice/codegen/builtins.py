"""Builtin function definitions and utilities for code generation."""

from mypy.nodes import NameExpr, StrExpr


def bool_expr(value: bool) -> NameExpr:
    """A True/False literal carrying the fullname the codegen matches on."""
    node = NameExpr("True" if value else "False")
    node.fullname = "builtins.True" if value else "builtins.False"
    return node


# Define builtin functions and their properties
BUILTINS = {
    "builtins.print": {
        "kwargs": ["sep", "end"],
        "defaults": {
            "sep": StrExpr(" "),
            "end": StrExpr("\n"),
        },
    },
    # sorted(iterable, *, key=None, reverse=False). key= is not supported yet.
    "builtins.sorted": {
        "kwargs": ["reverse"],
        "defaults": {
            "reverse": bool_expr(False),
        },
    },
}


def get_kwarg_order(fullname: str) -> list[str]:
    """Get the order of kwargs for a builtin function."""
    return BUILTINS[fullname]["kwargs"]


def get_kwarg_defaults(fullname: str) -> dict[str, StrExpr]:
    """Get default values for a builtin function's kwargs."""
    return BUILTINS[fullname]["defaults"]


def is_builtin_with_kwargs(fullname: str) -> bool:
    """Check if a function is a registered builtin with kwargs."""
    return fullname in BUILTINS


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
}

# A constructor call with no arguments (list(), set()) gives C++ template
# argument deduction nothing to work with, so these need their element type
# spelled out explicitly rather than left to CTAD.
EXPLICIT_TYPE_CONSTRUCTORS = {"list", "dict", "set"}

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
