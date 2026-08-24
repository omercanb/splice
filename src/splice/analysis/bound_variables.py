"""Save the variables that need to be declared inside each function"""

from mypy.nodes import (
    AssignmentStmt,
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


class _BoundVariableCollector(Traverser):
    """Collect all variables bound inside the node"""

    def __init__(self, types_dict: dict[Expression, Type]):
        self.types = types_dict
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

        elif isinstance(lvalue, (IndexExpr, MemberExpr)):
            # d[k] = ... / obj.attr = ... write into something that exists.
            pass
        else:
            assert False, "rejected by the static checks"

    def check_add_declaration(self, name: NameExpr):
        """Check if name is a declaration and add it to declarations"""
        if name.is_new_def:
            # TODO uncomment this after making comprehensions into functions
            # if name.name in self.declarations:
            #     print(name.name, self.declarations)
            # assert name.name not in self.declarations
            self.declarations[name.name] = self.types[name]
