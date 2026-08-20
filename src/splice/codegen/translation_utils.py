from __future__ import annotations

from typing import TYPE_CHECKING, Optional

from mypy.nodes import CallExpr, ComparisonExpr
from mypy.nodes import Expression
from mypy.nodes import Expression as MypyExpression
from mypy.nodes import FuncDef, LambdaExpr, NameExpr, TypeInfo
from mypy.types import CallableType, Type, get_proper_type

if TYPE_CHECKING:
    from splice.analysis.mutation import MutationTable

from splice.codegen.builtins import (
    BOOL_OP_MACROS,
    EXCEPTION_TYPES,
    EXPLICIT_TYPE_CONSTRUCTORS,
    METHOD_RENAMES,
    OP_MAP,
    QUALIFIED_BUILTINS,
    SCALAR_CONSTRUCTORS,
    get_kwarg_defaults,
    get_kwarg_order,
    is_builtin_with_kwargs,
)
from splice.codegen.typegen import cpp_type, cpp_type_name
from splice.visitor import Visitor


def get_function_type(func: FuncDef) -> CallableType:
    typ = get_proper_type(func.type)
    assert isinstance(typ, CallableType)
    return typ


def get_argument_names(func: FuncDef) -> list[str]:
    typ = get_function_type(func)
    args = typ.arg_names
    filtered_args: list[str] = []
    for arg in args:
        if arg is None:
            continue
        filtered_args.append(arg)
    return filtered_args


def translate_func_signature(
    o: FuncDef,
    expr_translator: Visitor[str],
    mutations: MutationTable,
    qualifier: str = "",
    flatten: bool = False,
) -> str:
    """Generate a C++ function signature. `qualifier` prefixes the name for a method's out-of-line definition; `flatten` is set for an @hotpath function."""
    func = get_function_type(o)
    return_type = cpp_type(func.ret_type)
    name = o.name
    if name == "main":
        return_type = "int"
    arguments = translate_parameters(o, expr_translator, mutations)
    prefix = "FLATTEN " if flatten else ""
    signature = f"{prefix}{return_type} {qualifier}{name}({', '.join(arguments)})"
    return signature


def translate_parameters(
    o: FuncDef, expr_translator: Visitor[str], mutations: MutationTable
) -> list[str]:
    """Every parameter is a reference and restrict. This is the whole advantage of using mutable value semantics"""
    func = get_function_type(o)
    arguments: list[str] = []
    for argument, argument_type in zip(o.arguments, func.arg_types):
        if argument.variable.is_self:
            continue
        argument_name = argument.variable.name
        argument_type_cpp = cpp_type(argument_type)
        const = "" if argument.variable in mutations else "const "
        ref_type = f"{const}{argument_type_cpp} &RESTRICT "
        if argument.initializer:
            default = expr_translator.visit(argument.initializer)
            s = f"{ref_type}{argument_name} = {default}"
        else:
            s = f"{ref_type}{argument_name}"
        arguments.append(s)
    return arguments


def translate_lambda_parameters(o: LambdaExpr) -> list[str]:
    params = []
    for argument in o.arguments:
        name = argument.variable.name
        s = f"auto {name}"
        params.append(s)
    return params


def parse_arguments(
    call: CallExpr,
) -> tuple[list[MypyExpression], dict[str, MypyExpression]]:
    positional_args: list[MypyExpression] = []
    kwargs: dict[str, MypyExpression] = {}

    for i, arg in enumerate(call.args):
        arg_name = call.arg_names[i]
        if arg_name is None:
            positional_args.append(arg)
        else:
            kwargs[arg_name] = arg
    return positional_args, kwargs


def has_kwargs(call: CallExpr):
    positional, kwargs = parse_arguments(call)
    if kwargs:
        return True
    else:
        return False


def translate_arguments_with_kwargs(
    call: CallExpr,
    expr_translator: Visitor[str],
) -> list[str]:
    """
    Reorder arguments for builtin functions by placing keyword arguments at the front.

    For print(a, b, sep=","), this transforms it to print(",", "\n", a, b)
    where the first two arguments are sep and end.
    """
    assert isinstance(call.callee, NameExpr)

    fullname = call.callee.fullname
    kwarg_names = get_kwarg_order(fullname)
    kwarg_defaults = get_kwarg_defaults(fullname)

    assert kwarg_names is not None, f"No kwarg order defined for {fullname}"
    assert kwarg_defaults is not None, f"No kwarg defaults defined for {fullname}"

    positional_args, kwargs = parse_arguments(call)

    new_args: list[str] = []

    # Add kwargs in the specified order with defaults
    for kwarg_name in kwarg_names:
        if kwarg_name in kwargs:
            new_args.append(expr_translator.visit(kwargs[kwarg_name]))
        else:
            default = kwarg_defaults[kwarg_name]
            new_args.append(expr_translator.visit(default))

    # Add positional arguments
    for positional_arg in positional_args:
        new_args.append(expr_translator.visit(positional_arg))

    return new_args


def should_translate_kwargs(call: CallExpr):
    """Returns wether a call is builtin AND uses kwargs"""
    return (
        has_kwargs(call)
        and isinstance(call.callee, NameExpr)
        and is_builtin_with_kwargs(call.callee.fullname)
    )


def translate_builtin_function_name_to_kwargs(o: CallExpr) -> str:
    """Translates the short name of the function, eg: print -> _print_kwargs"""
    assert isinstance(o.callee, NameExpr)
    name = o.callee.name
    return f"_{name}_kwargs"


def translate_qualified_builtin(callee: Expression) -> Optional[str]:
    """The C++ spelling for a builtin whose bare name would be ambiguous."""
    if isinstance(callee, NameExpr):
        return QUALIFIED_BUILTINS.get(callee.fullname)
    return None


def translate_constructor_special_cases(callee: Expression) -> Optional[str]:
    if not isinstance(callee, NameExpr):
        return
    # If it is a constructor
    if callee.name in SCALAR_CONSTRUCTORS:
        return SCALAR_CONSTRUCTORS[callee.name]
    return


def translate_tuple_access(i: int, base: str):
    # A tuple's elements have different types, so the index has to be a
    # compile-time one: t[0] becomes get<0>().
    return f"{member_access(base, f'get<{i}>')}()"


def needs_explicit_constructor_type(callee: Expression) -> bool:
    """A constructor call like list() or a class needs its C++ type spelled
    out rather than left to CTAD - see EXPLICIT_TYPE_CONSTRUCTORS."""
    if not isinstance(callee, NameExpr):
        return False
    if callee.name in EXPLICIT_TYPE_CONSTRUCTORS:
        return True
    # The builtin classes are spelled by hand above and in SCALAR_CONSTRUCTORS,
    # and str(x) has to stay to_str(x) rather than become a str constructor.
    return isinstance(callee.node, TypeInfo) and not callee.fullname.startswith(
        "builtins."
    )


def translate_constructor(t: Type, constructor: str):
    typ = cpp_type_name(t)
    return f"{typ}({constructor})"


def translate_membership(op: str, item: str, container: str) -> str:
    """`x in c` / `x not in c` -> c.__contains__(x), operands swapped."""
    call = call_method(container, "__contains__", item)
    return call if op == "in" else f"!{call}"


def translate_comparison(expr: ComparisonExpr, expr_translator: Visitor[str]):
    """Translate a python comparison like a < b < c into a < b && b < c"""
    pairwise_comparisons = expr.pairwise()
    terms = []  # Individual comaprisons to be connected by 'and'
    for op, expr1, expr2 in pairwise_comparisons:
        left = expr_translator.visit(expr1)
        right = expr_translator.visit(expr2)
        if op in ("in", "not in"):
            terms.append(translate_membership(op, left, right))
        else:
            terms.append(translate_binary_expr(op, left, right))
    full_comparison = " && ".join(terms)
    return f"({full_comparison})"


def translate_bool_op(op: str, left: str, right: str) -> str:
    """`a and b` / `a or b` where the resulting value is wanted, not a bool."""
    return f"{BOOL_OP_MACROS[op]}({left}, {right})"


def translate_binary_expr(op: str, expr1: str, expr2: str):
    if op in OP_MAP:
        return f"{OP_MAP[op]}({expr1}, {expr2})"
    else:
        return f"({expr1} {op} {expr2})"


def translate_method_name(name: str) -> str:
    """Python method name to its C++ spelling (set.union -> union_)."""
    return METHOD_RENAMES.get(name, name)


def member_access(obj: str, name: str) -> str:
    """`obj.name`"""
    return f"{obj}.{name}"


def call_method(obj: str, name: str, *args: str) -> str:
    """Call a method on a Python value, eg. `c.__contains__(x)`."""
    return f"{member_access(obj, name)}({', '.join(args)})"


def is_truthy(expr: str) -> str:
    """Wrap a C++ expression with Python's truthiness rules (bool()/`if`/`while`/`not`)."""
    return f"to_bool({expr})"
