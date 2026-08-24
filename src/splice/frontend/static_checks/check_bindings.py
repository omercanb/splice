"""Static checks for binding and reading variables
Two checks are needed
- Reading a for loop's variable after the loop binding it
- Reassigning to a function parameter
"""

from __future__ import annotations

from mypy.nodes import (
    AssignmentStmt,
    DictionaryComprehension,
    Expression,
    ForStmt,
    FuncDef,
    LambdaExpr,
    ListComprehension,
    Lvalue,
    MypyFile,
    NameExpr,
    OperatorAssignmentStmt,
    SetComprehension,
    TupleExpr,
)
from mypy.types import Instance, Type, get_proper_type

from splice.analysis.free_variables import get_free_variables
from splice.codegen.builtins import SCALAR_FULLNAMES
from splice.convert_to_python import convert_to_python
from splice.frontend.static_checks.compiler_errors_warnings import Diagnostic, diagnostic
from splice.visitor import Traverser


def _lvalue_names(lvalue: Lvalue) -> list[str]:
    if isinstance(lvalue, NameExpr):
        return [lvalue.name]
    if isinstance(lvalue, TupleExpr):
        return [name for item in lvalue.items for name in _lvalue_names(item)]
    return []


class _BindingChecker(Traverser):
    def __init__(self, types: dict[Expression, Type]) -> None:
        self.types = types
        self.diagnostics: list[Diagnostic] = []
        self.closed_loop_targets: dict[str, ForStmt] = {}
        self.parameter_names: set[str] = set()

    def report(self, node, kind: str, message: str, hint: str) -> None:
        self.diagnostics.append(diagnostic(node, kind, message, hint))

    def _report_parameter_reassignment(self, lvalue: NameExpr) -> None:
        self.report(
            lvalue,
            "parameter-reassignment",
            f"`{lvalue.name}` is a parameter and can't be reassigned",
            "Reassigning a parameter doesn't fit with C++, where a parameter compiles to a reference. "
            "Assign the new value to a differently-named local instead",
        )

    def _is_scalar(self, expr: Expression) -> bool:
        t = get_proper_type(self.types.get(expr))
        return isinstance(t, Instance) and t.type.fullname in SCALAR_FULLNAMES

    def visit_func_def(self, o: FuncDef) -> None:
        # A nested def gets its own set of both, restored on exit - same reasoning
        # as check_structural.py's original: one function's bindings shouldn't leak into
        # an unrelated sibling or enclosing function.
        outer_loop_targets = self.closed_loop_targets
        outer_parameters = self.parameter_names
        self.closed_loop_targets = {}
        self.parameter_names = {argument.variable.name for argument in o.arguments}
        super().visit_func_def(o)
        self.closed_loop_targets = outer_loop_targets
        self.parameter_names = outer_parameters

    def _bind(self, lvalue: Lvalue) -> None:
        """A name is being freshly bound - a plain assignment, or a for loop's own
        index. No longer stale, and an error if it's the function's own parameter.
        """
        if isinstance(lvalue, NameExpr):
            self.closed_loop_targets.pop(lvalue.name, None)
            if lvalue.name in self.parameter_names:
                self._report_parameter_reassignment(lvalue)
            return
        if isinstance(lvalue, TupleExpr):
            for item in lvalue.items:
                self._bind(item)
            return

    def visit_assignment_stmt(self, o: AssignmentStmt) -> None:
        for lvalue in o.lvalues:
            self._bind(lvalue)
        super().visit_assignment_stmt(o)

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> None:
        # x += 1 is dangerous if x is an int because it will mutate the original variable not the parameter
        # We need to check that as well
        if (
            isinstance(o.lvalue, NameExpr)
            and o.lvalue.name in self.parameter_names
            and self._is_scalar(o.lvalue)
        ):
            self._report_parameter_reassignment(o.lvalue)
        super().visit_operator_assignment_stmt(o)

    def visit_for_stmt(self, o: ForStmt) -> None:
        self._bind(o.index)
        self.visit(o.index)
        self.visit(o.expr)
        self.visit(o.body)
        for name in _lvalue_names(o.index):
            self.closed_loop_targets[name] = o
        if o.else_body is not None:
            self.visit(o.else_body)

    def _report_if_invalid_access(self, o: NameExpr) -> None:
        """Report an access to a name if it's a name that was defined by a now closed for loop."""
        closing = self.closed_loop_targets.get(o.name)
        if closing is not None:
            capture = f"{o.name}_last"
            self.report(
                o,
                "stale-loop-variable",
                f"`{o.name}` is read after the `for` loop that bound it has ended",
                "Accessing the for loop iterator after the loop is not compatible with C++. "
                "Assign it inside the loop to another varibale to use it after the loop.\n"
                f"for {convert_to_python(closing.index)} in "
                f"{convert_to_python(closing.expr)}:\n"
                f"    {capture} = {o.name}\n"
                f"print({capture})",
            )

    def visit_name_expr(self, o: NameExpr) -> None:
        self._report_if_invalid_access(o)

    def _visit_comprehension(self, o, generator, elements: list[Expression]) -> None:
        # A comprehension's or lambda's own bound names shadow
        for free in get_free_variables(o):
            self._report_if_invalid_access(free)

        bound_names = {
            name for index in generator.indices for name in _lvalue_names(index)
        }
        saved_loops = {
            name: self.closed_loop_targets.pop(name, None) for name in bound_names
        }
        saved_params = self.parameter_names & bound_names
        self.parameter_names -= bound_names
        for index in generator.indices:
            self._bind(index)
            self.visit(index)
        for sequence in generator.sequences:
            self.visit(sequence)
        for conditions in generator.condlists:
            for condition in conditions:
                self.visit(condition)
        for element in elements:
            self.visit(element)
        for name, prior in saved_loops.items():
            if prior is not None:
                self.closed_loop_targets[name] = prior
        self.parameter_names |= saved_params

    def visit_list_comprehension(self, o: ListComprehension) -> None:
        self._visit_comprehension(o, o.generator, [o.generator.left_expr])

    def visit_set_comprehension(self, o: SetComprehension) -> None:
        self._visit_comprehension(o, o.generator, [o.generator.left_expr])

    def visit_dictionary_comprehension(self, o: DictionaryComprehension) -> None:
        self._visit_comprehension(o, o, [o.key, o.value])

    def visit_lambda_expr(self, o: LambdaExpr) -> None:
        for free in get_free_variables(o):
            self._report_if_invalid_access(free)

        bound_names = {argument.variable.name for argument in o.arguments}
        saved_loops = {
            name: self.closed_loop_targets.pop(name, None) for name in bound_names
        }
        saved_params = self.parameter_names & bound_names
        self.parameter_names -= bound_names
        for argument in o.arguments:
            if argument.initializer is not None:
                self.visit(argument.initializer)
        self.visit(o.body)
        for name, prior in saved_loops.items():
            if prior is not None:
                self.closed_loop_targets[name] = prior
        self.parameter_names |= saved_params


def check_bindings(tree: MypyFile, types: dict[Expression, Type]) -> list[Diagnostic]:
    checker = _BindingChecker(types)
    checker.visit(tree)
    return checker.diagnostics
