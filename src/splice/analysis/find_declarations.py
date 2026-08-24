"""Save the variables that need to be declared inside each function"""

from mypy.nodes import (
    AssignmentStmt,
    CallExpr,
    Expression,
    ForStmt,
    FuncDef,
    IndexExpr,
    Lvalue,
    MemberExpr,
    NameExpr,
    TupleExpr,
)
from mypy.types import CallableType, Type, get_proper_type

from splice.visitor import Traverser


class _DeclarationCollector(Traverser):
    """Collect all local variable declarations in a function."""

    def __init__(self, types_dict: dict[Expression, Type], parameter_names: set[str]):
        self.types = types_dict
        self.parameter_names = parameter_names
        self.declarations: dict[str, Type] = {}

    def visit_assignment_stmt(self, o: AssignmentStmt) -> None:
        """Collect variables from assignment statements."""
        for lvalue in o.lvalues:
            self.check_names(lvalue)
        self.visit(o.rvalue)

    def visit_for_stmt(self, o: ForStmt) -> None:
        self.check_names(o.index)
        # Keep walking: a loop body can declare names too, including the
        # index of a nested loop.
        super().visit_for_stmt(o)

    def check_names(self, lvalue: Lvalue) -> None:
        """Recursively collect all names in an lvalue."""
        if isinstance(lvalue, NameExpr):
            # Simple: x = ...
            self.check_add_declaration(lvalue)

        elif isinstance(lvalue, TupleExpr):
            # Destructuring: x, y = ...
            for item in lvalue.items:
                self.check_names(item)

        elif isinstance(lvalue, (IndexExpr, MemberExpr, CallExpr)):
            # d[k] = ... / obj.attr = ... / a.back() = ... write into
            # something that exists.
            pass
        else:
            assert False, "rejected by the static checks"

    def check_add_declaration(self, name: NameExpr):
        """Check if name is a declaration and add it to declarations"""
        if name.name in self.parameter_names:
            # Already declared as a function parameter.
            return
        if name.is_new_def:
            # TODO uncomment this after making comprehensions into functions
            # if name.name in self.declarations:
            #     print(name.name, self.declarations)
            # assert name.name not in self.declarations
            self.declarations[name.name] = self.types[name]


def get_declarations(
    func: FuncDef, types_dict: dict[Expression, Type]
) -> dict[str, Type]:
    parameter_names = {argument.variable.name for argument in func.arguments}
    collector = _DeclarationCollector(types_dict, parameter_names)
    collector.visit(func)
    return collector.declarations
