"""Reject Python the transpiler cannot translate, before code generation.

Codegen may then assume its input is translatable, so what is left there are
invariants rather than user facing checks. Every construct is collected in one
walk so a program reports all of its problems at once.
"""

from __future__ import annotations

from dataclasses import dataclass
from enum import StrEnum
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from splice.ast_utils import TypeTable

from mypy.nodes import (
    AssignmentStmt,
    Block,
    CallExpr,
    ClassDef,
    DictExpr,
    DictionaryComprehension,
    Expression,
    ExpressionStmt,
    ForStmt,
    FuncDef,
    GeneratorExpr,
    GlobalDecl,
    IfStmt,
    Decorator,
    IndexExpr,
    IntExpr,
    LambdaExpr,
    ListComprehension,
    ListExpr,
    Lvalue,
    MemberExpr,
    MypyFile,
    NameExpr,
    NonlocalDecl,
    OperatorAssignmentStmt,
    OpExpr,
    PassStmt,
    RaiseStmt,
    ReturnStmt,
    SetComprehension,
    SetExpr,
    SliceExpr,
    StarExpr,
    StrExpr,
    TempNode,
    TryStmt,
    TupleExpr,
    UnaryExpr,
    Var,
    WhileStmt,
    YieldExpr,
    YieldFromExpr, Context,
)
from mypy.types import (
    CallableType,
    Instance,
    ProperType,
    TupleType,
    Type,
    UnionType,
    get_proper_type,
)

from splice.analysis.free_variables import get_free_variables
from splice.ast_utils import literal_int_value
from splice.codegen.builtins import EXCEPTION_TYPES, OP_MAP
from splice.codegen.exceptions import names_a_class
from splice.codegen.typegen import UnsupportedType, cpp_type, cpp_type_name
from splice.convert_to_python import convert_to_python
from splice.visitor import Traverser

SUPPORTED_EXCEPTIONS = ", ".join(
    sorted({name.rpartition(".")[2] for name in EXCEPTION_TYPES})
)


class _ReturnFinder(Traverser):
    def __init__(self) -> None:
        self.found = False

    def visit_return_stmt(self, o: ReturnStmt) -> None:
        self.found = True


def returns(block: Block) -> bool:
    finder = _ReturnFinder()
    finder.visit(block)
    return finder.found


class _CallFinder(Traverser):
    def __init__(self) -> None:
        self.found = False

    def visit_call_expr(self, o: CallExpr) -> None:
        self.found = True
        super().visit_call_expr(o)


def _contains_call(expression: Expression) -> bool:
    finder = _CallFinder()
    finder.visit(expression)
    return finder.found


def _lvalue_names(lvalue: Lvalue) -> list[str]:
    """The names a target binds, for tracking what a `for` loop leaves stale."""
    if isinstance(lvalue, NameExpr):
        return [lvalue.name]
    if isinstance(lvalue, TupleExpr):
        return [name for item in lvalue.items for name in _lvalue_names(item)]
    return []


def _negative_int_literal(expr: Expression) -> int | None:
    """The value of a literal negative int (`-1`), which parses as a unary
    minus applied to a positive IntExpr rather than as a negative IntExpr.
    """
    if (
        isinstance(expr, UnaryExpr)
        and expr.op == "-"
        and isinstance(expr.expr, IntExpr)
    ):
        return -expr.expr.value
    return None


def _type_hint(t: Type) -> str:
    """Holding a function in a variable is the common way to land here."""
    proper = get_proper_type(t)
    if isinstance(proper, CallableType):
        return (
            "a function cannot be stored in a variable, call it where it is "
            "needed instead:\nprint(add(1, 2))"
        )
    return (
        "use int, float, str, bool, or a list, dict, set or tuple of those:\n"
        "values: list[int] = []"
    )


def _name_of(expression: Expression, fallback: str) -> str:
    """The written name, for hints that quote the code back at the reader."""
    return expression.name if isinstance(expression, NameExpr) else fallback


def _subject_name(node) -> str | None:
    """The name behind a type-check target, when the node has one at all.

    check_type() is called against several different kinds of node (a
    FuncDef for a return type, an Argument for a parameter, a Var for a
    module-level symbol, any Expression for everything else) - only some of
    which carry a name to quote back at the reader.
    """
    if isinstance(node, (NameExpr, Var, FuncDef)):
        return node.name
    variable = getattr(node, "variable", None)
    if variable is not None:
        return variable.name
    return None


def _tuple_index_hint(o: IndexExpr) -> str:
    base = _name_of(o.base, "pair")
    index = _name_of(o.index, "i")
    return (
        "a tuple holds a different type at each position, so the position has "
        "to be known at compile time.\n"
        f"Index it with a literal:\n"
        f"{base}[0]\n"
        f"or hold the values in a list, where every element has one type:\n"
        f"{base} = [1, 2]\n"
        f"print({base}[{index}])"
    )


class _Validator(Traverser):
    def __init__(self, types: dict[Expression, Type]):
        self.types = types
        self.diagnostics: list[Diagnostic] = []
        # Names a `for` loop has finished with, in the current function - read
        # after that point in program order is not a translation this project
        # matches, since C++'s for loop leaves its counter one past the value
        # Python's would.
        self.closed_loop_targets: dict[str, ForStmt] = {}

    def report(self, node, kind: str, message: str, hint: str) -> None:
        self.diagnostics.append(diagnostic(node, kind, message, hint))

    def check_type(self, node, t: Type) -> None:
        """Report a type codegen has no C++ spelling for."""
        try:
            cpp_type(t)
        except UnsupportedType as unsupported:
            if str(unsupported.problematic_member_type) == "object":
                name = _subject_name(node)
                subject = f"`{name}`" if name else "this expression"
                self.report(
                    node,
                    "object type",
                    f"Type of expression is inferred as `{unsupported.type}` which contains `object`. This cannot be translated to C++.",
                    "Everything must have strictly one type only. There may be multiple different types being assigned to the field with type `object`. "
                    f"You can give {subject} a type annotation to help you catch the error.",
                )
            else:
                self.report(
                    node,
                    "unsupported-type",
                    f"no C++ equivalent for the type `{unsupported.type}`",
                    _type_hint(unsupported.type),
                )

    def check_inferred_type(self, node: Expression) -> None:
        t = self.types.get(node)
        if t is not None and isinstance(t, ProperType):
            self.check_type(node, t)

    def visit_class_def(self, o: ClassDef) -> None:
        if o.base_type_exprs:
            self.report(
                o,
                "class-inheritance",
                "a base class is not supported",
                "give the class its own copy of what it needs:\n"
                f"class {o.name}:\n"
                "    def __init__(self, value: int) -> None:\n"
                "        self.value = value",
            )
        for statement in o.defs.body:
            self.check_class_member(statement)
        super().visit_class_def(o)

    def check_class_member(self, statement) -> None:
        """A class body holds annotations and methods, and nothing else."""
        # A decorator is ignored, not translated - the underlying method
        # still gets visited normally via visit_decorator's traversal.
        if isinstance(statement, (FuncDef, PassStmt, Decorator)):
            return
        # A docstring is an expression statement; it is simply dropped.
        if isinstance(statement, ExpressionStmt) and isinstance(
            statement.expr, StrExpr
        ):
            return
        if isinstance(statement, AssignmentStmt):
            # `x: int` parses as an assignment whose value is a placeholder.
            if isinstance(statement.rvalue, TempNode):
                return
            name = convert_to_python(statement.lvalues[0])
            value = convert_to_python(statement.rvalue)
            self.report(
                statement,
                "class-variable",
                "a class level value is not supported",
                "every attribute is per instance, so set it in __init__:\n"
                "def __init__(self) -> None:\n"
                f"    self.{name} = {value}",
            )
            return
        self.report(
            statement,
            "class-body",
            "only attributes and methods are supported in a class body",
            "move anything else outside the class",
        )

    def visit_func_def(self, o: FuncDef) -> None:
        signature = get_proper_type(o.type)
        if isinstance(signature, CallableType):
            self.check_type(o, signature.ret_type)
        for argument in o.arguments:
            if argument.variable.type is not None:
                self.check_type(argument, argument.variable.type)
        # A nested def gets its own set of stale loop targets, restored on exit.
        outer_closed = self.closed_loop_targets
        self.closed_loop_targets = {}
        super().visit_func_def(o)
        self.closed_loop_targets = outer_closed

    def visit_assignment_stmt(self, o: AssignmentStmt) -> None:
        source = convert_to_python(o.rvalue)
        for lvalue in o.lvalues:
            self.check_lvalue(lvalue, source)
        if len(o.lvalues) > 1:
            first, *rest = o.lvalues
            first_name = convert_to_python(first)
            copies = "\n".join(
                f"{convert_to_python(target)} = {first_name}" for target in rest
            )
            self.report(
                o,
                "chained-assignment",
                "assigning to multiple targets in one statement is not supported",
                "assign the value once, then copy it to the rest:\n"
                f"{first_name} = {convert_to_python(o.rvalue)}\n"
                f"{copies}",
            )
        if isinstance(o.lvalues[0], TupleExpr):
            if o.rvalue in self.types:
                rhs = self.types[o.rvalue]
                if isinstance(rhs, Instance) and rhs.type.fullname == "builtins.list":
                    targets = o.lvalues[0].items
                    source = convert_to_python(o.rvalue)
                    if any(isinstance(target, StarExpr) for target in targets):
                        # A starred target needs a slice, not a single index;
                        # the starred-assignment check explains that part.
                        hint = "Assign the right hand side elements one by one."
                    else:
                        lines = "\n".join(
                            f"{convert_to_python(target)} = {source}[{i}]"
                            for i, target in enumerate(targets)
                        )
                        hint = (
                            f"Assign the right hand side elements one by one:\n{lines}"
                        )
                    self.report(
                        o,
                        "assignment",
                        "Can't use desugaring assignment on a list.",
                        hint,
                    )
        super().visit_assignment_stmt(o)

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> None:
        if o.op in OP_MAP:
            # These compile to `lhs = f(lhs, rhs)`, since C++ has no direct
            # compound form for them - lhs's text would then appear twice,
            # so a side-effecting target (a[f()] //= 2) would evaluate that
            # side effect twice, and could even read one element and write
            # a different one if f() isn't idempotent.
            lvalue = convert_to_python(o.lvalue)
            rvalue = convert_to_python(o.rvalue)
            self.report(
                o,
                "compound-assign-op",
                f"`{o.op}=` is not supported",
                "compute the target once, then write it out normally:\n"
                f"{lvalue} = {lvalue} {o.op} ({rvalue})",
            )
        super().visit_operator_assignment_stmt(o)

    def check_lvalue(self, lvalue: Lvalue, source: str | None = None) -> None:
        if isinstance(lvalue, NameExpr):
            # A fresh bind, whether new or a rebind, is no longer stale.
            self.closed_loop_targets.pop(lvalue.name, None)
            if lvalue.is_new_def:
                self.check_inferred_type(lvalue)
            return
        if isinstance(lvalue, TupleExpr):
            for item in lvalue.items:
                self.check_lvalue(item, source)
            return
        if isinstance(lvalue, StarExpr):
            self.report(
                lvalue,
                "starred-assignment",
                "a starred assignment target is not supported",
                "take the parts by slicing instead:\n"
                "first = values[0]\nrest = values[1:]",
            )
            return
        if isinstance(lvalue, IndexExpr) and isinstance(lvalue.index, SliceExpr):
            base = convert_to_python(lvalue.base)
            begin = lvalue.index.begin_index
            start = convert_to_python(begin) if begin is not None else "0"
            values = source if source is not None else "values"
            self.report(
                lvalue,
                "slice-assignment",
                "assigning to a slice is not supported",
                "assign the target elements one by one instead:\n"
                f"{base}[{start}] = {values}[0]\n{base}[{start} + 1] = {values}[1]",
            )
            return
        # Writing into something that already exists, nothing to declare.
        assert isinstance(lvalue, (IndexExpr, MemberExpr)), lvalue

    def visit_try_stmt(self, o: TryStmt) -> None:
        if o.is_star:
            # mypy records no span for the `except*` token itself, so point at
            # the first handler's class rather than at `try`.
            handled = next((t for t in o.types if t is not None), o)
            first_var = next((v for v in o.vars if v is not None), None)
            klass = convert_to_python(handled) if handled is not o else "ValueError"
            as_clause = f" as {first_var.name}" if first_var is not None else ""
            self.report(
                handled,
                "except-star",
                "`except*` groups are not supported",
                f"use a plain except clause:\nexcept {klass}{as_clause}:",
            )
        for type_expression in o.types:
            self.check_handler_type(type_expression)
        if o.finally_body is not None and returns(o.finally_body):
            self.report(
                o.finally_body,
                "return-in-finally",
                "`return` inside `finally` is not supported",
                "return after the try statement instead:\n"
                "try:\n    ...\nfinally:\n    cleanup()\nreturn result",
            )
        super().visit_try_stmt(o)

    def check_handler_type(self, type_expression: Expression | None) -> None:
        if type_expression is None:
            return
        if isinstance(type_expression, TupleExpr):
            example = "\n".join(
                f"except {convert_to_python(item)}:\n    ..."
                for item in type_expression.items
            )
            self.report(
                type_expression,
                "except-tuple",
                "an except clause takes a single exception class",
                f"write one clause per class:\n{example}",
            )
            return
        self.check_exception_class(type_expression)

    def visit_raise_stmt(self, o: RaiseStmt) -> None:
        if o.from_expr is not None:
            example = (
                convert_to_python(o.expr) if o.expr is not None else "ValueError()"
            )
            self.report(
                o,
                "raise-from",
                "chaining exceptions with `from` is not supported",
                f"raise the new exception on its own:\nraise {example}",
            )
        raised = o.expr
        if raised is not None:
            klass = raised.callee if isinstance(raised, CallExpr) else raised
            self.check_exception_class(klass)
            if isinstance(raised, CallExpr) and len(raised.args) > 1:
                joined = " + ".join(convert_to_python(a) for a in raised.args)
                self.report(
                    raised,
                    "exception-arguments",
                    "an exception takes a single message",
                    f"join the parts into one string:\n"
                    f"raise {convert_to_python(klass)}({joined})",
                )
        super().visit_raise_stmt(o)

    def check_exception_class(self, expression: Expression) -> None:
        """Only classes are checked; a variable holds an already valid one."""
        if not names_a_class(expression):
            return
        assert isinstance(expression, NameExpr)
        if expression.fullname not in EXCEPTION_TYPES:
            self.report(
                expression,
                "unsupported-exception",
                f"`{expression.name}` has no C++ equivalent",
                f"use one of: {SUPPORTED_EXCEPTIONS}\n"
                "a base class like LookupError has to be spelled as the "
                "concrete classes it covers",
            )

    def visit_dict_expr(self, o: DictExpr) -> None:
        unpacked = next((value for key, value in o.items if key is None), None)
        if unpacked is not None:
            kept = ", ".join(
                f"{convert_to_python(key)}: {convert_to_python(value)}"
                for key, value in o.items
                if key is not None
            )
            source = convert_to_python(unpacked)
            self.report(
                o,
                "dict-unpacking",
                "`**` unpacking in a dict literal is not supported",
                "copy the entries across in a loop:\n"
                f"merged = {{{kept}}}\n"
                f"for key in {source}:\n    merged[key] = {source}[key]",
            )
        self.check_inferred_type(o)
        super().visit_dict_expr(o)

    # A list or set comprehension holds a GeneratorExpr, so its parts are
    # walked directly; anything that still reaches visit_generator_expr is a
    # bare generator.
    def visit_list_comprehension(self, o: ListComprehension) -> None:
        self.check_inferred_type(o)
        self._visit_comprehension(o, o.generator, [o.generator.left_expr])

    def visit_set_comprehension(self, o: SetComprehension) -> None:
        self.check_inferred_type(o)
        self._visit_comprehension(o, o.generator, [o.generator.left_expr])

    def visit_dictionary_comprehension(self, o: DictionaryComprehension) -> None:
        self.check_inferred_type(o)
        self._visit_comprehension(o, o, [o.key, o.value])

    def _visit_comprehension(self, o, generator, elements: list[Expression]) -> None:
        for free in get_free_variables(o):
            self._report_if_stale(free)

        bound_names = {
            name for index in generator.indices for name in _lvalue_names(index)
        }
        saved = {name: self.closed_loop_targets.pop(name, None) for name in bound_names}
        for index in generator.indices:
            self.check_lvalue(index)
            self.visit(index)
        for sequence in generator.sequences:
            self.visit(sequence)
        for conditions in generator.condlists:
            for condition in conditions:
                self.visit(condition)
        for element in elements:
            self.visit(element)
        for name, prior in saved.items():
            if prior is not None:
                self.closed_loop_targets[name] = prior

    def visit_lambda_expr(self, o: LambdaExpr) -> None:
        for free in get_free_variables(o):
            self._report_if_stale(free)

        bound_names = {argument.variable.name for argument in o.arguments}
        saved = {name: self.closed_loop_targets.pop(name, None) for name in bound_names}
        for argument in o.arguments:
            if argument.initializer is not None:
                self.visit(argument.initializer)
        self.visit(o.body)
        for name, prior in saved.items():
            if prior is not None:
                self.closed_loop_targets[name] = prior

    def visit_generator_expr(self, o: GeneratorExpr) -> None:
        as_list = convert_to_python(ListComprehension(o))
        self.report(
            o,
            "generator-expression",
            "generator expressions are not supported",
            "wrap it in a list, which is built in one go rather than lazily:\n"
            f"total = sum({as_list})",
        )

    def visit_yield_expr(self, o: YieldExpr) -> None:
        self.report(
            o,
            "generator-expression",
            "generators aren't supported",
            "return the values in a list instead of yielding them:\n"
            "def values() -> list[int]:\n"
            "    return [1, 2, 3]",
        )

    def visit_yield_from_expr(self, o: YieldFromExpr) -> None:
        self.visit_yield_expr(o)

    def visit_global_decl(self, o: GlobalDecl) -> None:
        self.report(
            o,
            "global-declaration",
            "the global keyword is not supported",
            "globals are constexpr and can't be reassigned - pass "
            f"{', '.join(o.names)} in as a parameter instead",
        )

    def visit_nonlocal_decl(self, o: NonlocalDecl) -> None:
        self.report(
            o,
            "nonlocal-declaration",
            "the nonlocal keyword is not supported",
            "return the new value instead of writing through an outer scope",
        )

    def visit_list_expr(self, o: ListExpr) -> None:
        self.check_inferred_type(o)
        super().visit_list_expr(o)

    def visit_set_expr(self, o: SetExpr) -> None:
        self.check_inferred_type(o)
        super().visit_set_expr(o)

    def visit_index_expr(self, o: IndexExpr) -> None:
        base_type = get_proper_type(self.types.get(o.base))
        if isinstance(base_type, TupleType):
            value = literal_int_value(self.types.get(o.index))
            if value is None or value < 0:
                self.report(
                    o.index,
                    "tuple-index",
                    "a tuple can only be indexed by an integer literal",
                    _tuple_index_hint(o),
                )
        else:
            value = _negative_int_literal(o.index)
            if value is not None and value != -1:
                base = convert_to_python(o.base)
                self.report(
                    o.index,
                    "negative-index",
                    "a negative index other than -1 is not supported",
                    f"index from the front instead:\n{base}[len({base}) - {-value}]",
                )
        super().visit_index_expr(o)

    def visit_if_stmt(self, o: IfStmt) -> None:
        for condition in o.expr:
            self.visit_condition(condition)
        for body in o.body:
            self.visit(body)
        if o.else_body is not None:
            self.visit(o.else_body)

    def visit_while_stmt(self, o: WhileStmt) -> None:
        self.visit_condition(o.expr)
        self.visit(o.body)
        if o.else_body is not None:
            self.visit(o.else_body)

    def visit_for_stmt(self, o: ForStmt) -> None:
        self.check_range_start(o)
        self.check_lvalue(o.index)
        self.visit(o.index)
        self.visit(o.expr)
        self.visit(o.body)
        for name in _lvalue_names(o.index):
            self.closed_loop_targets[name] = o
        if o.else_body is not None:
            self.visit(o.else_body)

    def check_range_start(self, o: ForStmt) -> None:
        """range()'s stop/step get evaluated before start once this compiles,
        so a call in start could run in the wrong order relative to them.
        """
        iterable = o.expr
        if not (
            isinstance(iterable, CallExpr)
            and isinstance(iterable.callee, NameExpr)
            and iterable.callee.name == "range"
            and len(iterable.args) >= 2
        ):
            return
        start = iterable.args[0]
        if _contains_call(start):
            rest = ", ".join(convert_to_python(a) for a in iterable.args[1:])
            self.report(
                start,
                "range-start-side-effect",
                "a function call in range()'s start argument is not supported",
                "evaluate it into a variable first, so it runs before "
                "range()'s other arguments do:\n"
                f"start = {convert_to_python(start)}\n"
                f"for {convert_to_python(o.index)} in range(start, {rest}):",
            )

    def visit_name_expr(self, o: NameExpr) -> None:
        self._report_if_stale(o)

    def _report_if_stale(self, o: NameExpr) -> None:
        closing = self.closed_loop_targets.get(o.name)
        if closing is not None:
            capture = f"{o.name}_last"
            self.report(
                o,
                "stale-loop-variable",
                f"`{o.name}` is read after the `for` loop that bound it has ended",
                "C++'s for loop leaves its counter one past the last value "
                "used, unlike Python, so this would read a different value "
                "here. Assign it to another variable inside the loop body "
                "instead:\n"
                f"for {convert_to_python(closing.index)} in "
                f"{convert_to_python(closing.expr)}:\n"
                f"    {capture} = {o.name}\n"
                f"print({capture})",
            )

    def visit_condition(self, expression: Expression) -> None:
        """Walk an expression in condition position.

        Mirrors ExpressionCodegen.condition: and/or/not there become &&/||/!,
        which need no value at all, so the operands never need a common type.
        """
        if isinstance(expression, OpExpr) and expression.op in ("and", "or"):
            self.visit_condition(expression.left)
            self.visit_condition(expression.right)
            return
        if isinstance(expression, UnaryExpr) and expression.op == "not":
            self.visit_condition(expression.expr)
            return
        self.visit(expression)

    def visit_op_expr(self, o: OpExpr) -> None:
        if o.op in ("and", "or"):
            self.check_bool_op(o)
        super().visit_op_expr(o)

    def check_bool_op(self, o: OpExpr) -> None:
        result = get_proper_type(self.types.get(o))
        if not isinstance(result, UnionType):
            return
        try:
            spellings = {cpp_type_name(item) for item in result.items}
        except UnsupportedType:
            return  # reported against the operand that carries the type
        if len(spellings) > 1:
            self.report(
                o,
                "mixed-bool-op",
                f"`{o.op}` on unrelated types has no single C++ type",
                f"`{o.op}` returns one of its operands, so both sides need the "
                "same type.\nUse it directly in a condition instead:\n"
                f"if {convert_to_python(o.left)} {o.op} {convert_to_python(o.right)}:",
            )

    def visit_comparison_expr(self, o: ComparisonExpr) -> None:
        for op, left, right in o.pairwise():
            if op in ("is", "is not"):
                replacement = "==" if op == "is" else "!="
                self.report(
                    o,
                    "identity-comparison",
                    f"`{op}` doesn't make sense once compiled to C++",
                    "every value is independent once compiled, so there's no "
                    f"separate notion of identity - use {replacement} if you "
                    "meant to compare values:\n"
                    f"{convert_to_python(left)} {replacement} {convert_to_python(right)}",
                )
        super().visit_comparison_expr(o)


class Severity(StrEnum):
    ERROR = "error"
    WARNING = "warning"


@dataclass(frozen=True)
class Diagnostic:
    """One unsupported construct and the source span it occupies."""

    kind: str
    message: str
    hint: str
    line: int
    column: int
    end_line: int
    end_column: int
    # WARNING reports but never blocks compilation, unlike the default.
    severity: Severity = Severity.ERROR

    @property
    def position(self) -> tuple[int, int]:
        return self.line, self.column


def validate(tree: MypyFile, types: TypeTable) -> list[Diagnostic]:
    """Every construct in the file that cannot be translated, in source order."""
    type_table: dict[Expression, ProperType] = {}
    for k, v in types.items():
        if isinstance(v, ProperType):
            type_table[k] = v
    validator = _Validator(type_table)
    validator.visit(tree)
    for symbol in tree.names.values():
        if isinstance(symbol.node, Var) and symbol.type is not None:
            validator.check_type(symbol.node, symbol.type)
    # Nested and/or report once each, which reads as the same complaint twice.
    unique = {(d.position, d.kind): d for d in validator.diagnostics}
    return sorted(unique.values(), key=lambda d: d.position)


def diagnostic(
    node: Context,
    kind: str,
    message: str,
    hint: str,
    severity: Severity = Severity.ERROR,
) -> Diagnostic:
    """Build a diagnostic from any mypy node, which carries its own span."""
    end_line = node.end_line if node.end_line is not None else node.line
    end_column = node.end_column if node.end_column is not None else node.column + 1
    return Diagnostic(
        kind=kind,
        message=message,
        hint=hint,
        line=node.line,
        column=node.column,
        end_line=end_line,
        end_column=end_column,
        severity=severity,
    )


class UnsupportedProgram(Exception):
    """Every construct validation rejected, reported in one go."""

    def __init__(self, diagnostics: list[Diagnostic]):
        self.diagnostics = diagnostics
        super().__init__(f"{len(diagnostics)} unsupported construct(s)")


def render(
    diagnostics: list[Diagnostic], source: str, path: str = "<source>"
) -> str:
    """Render diagnostics with the offending source line underlined."""
    lines = source.splitlines()
    return "\n".join(_render_one(d, lines, path) for d in diagnostics)


def _render_one(diagnostic: Diagnostic, lines: list[str], path: str) -> str:
    text = lines[diagnostic.line - 1] if diagnostic.line <= len(lines) else ""
    # A span running onto later lines is underlined to the end of the first.
    end = diagnostic.end_column if diagnostic.end_line == diagnostic.line else len(text)
    underline = " " * diagnostic.column + "^" * max(1, end - diagnostic.column)

    number = str(diagnostic.line)
    gutter = " " * len(number)
    # mypy columns are 0 based, editors and compilers count from 1.
    header = (
        f"{path}:{diagnostic.line}:{diagnostic.column + 1}: "
        f"{diagnostic.severity}: {diagnostic.message}"
    )
    hint_lines = diagnostic.hint.splitlines()
    hint = "\n".join(
        [f"  help: {hint_lines[0]}"] + [f"          {l}" for l in hint_lines[1:]]
    )
    return (
        f"{header}\n"
        f"\n"
        f"  {number} | {text}\n"
        f"  {gutter} | {underline}\n"
        f"\n"
        f"{hint}\n"
    )
