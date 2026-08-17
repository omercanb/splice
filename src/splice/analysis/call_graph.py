"""Build a call graph"""

# Walk the whole tree
# Create a graph of funcdefs to funcdefs with parameter bindings as the edges


from dataclasses import dataclass, field
from typing import Optional

from mypy.nodes import CallExpr, Expression, FuncDef, MemberExpr, NameExpr, Var
from mypy.types import Instance

from splice.pipeline import TypeTable
from splice.visitor import Traverser


@dataclass
class Edge:
    callee: FuncDef
    bindings: list[tuple[Expression, Var]] = field(default_factory=list)


@dataclass
class CallGraph:
    calls: dict[FuncDef, Edge] = field(default_factory=dict)


class CallGraphProducer(Traverser):
    def __init__(self):
        self.current_function: Optional[FuncDef] = None

    def visit_func_def(self, o: FuncDef):
        self.current_function = o
        return super().visit_func_def(o)

    def visit_call_expr(self, o: CallExpr):

        # If current function add edge to call graph
        # Note: The function needs to be a node in the graph
        return super().visit_call_expr(o)

    def visit_member_expr(self, o: MemberExpr):
        # If you're a call, add edge to call graph with self as the first parameter
        return super().visit_member_expr(o)


def is_call_builtin(o: CallExpr, types: TypeTable):
    if isinstance(o.callee, NameExpr):
        return o.callee.fullname.startswith("builtins.")
    if isinstance(o.callee, MemberExpr):
        t = types[o.callee.expr]
        assert isinstance(t, Instance)
        return t.type.fullname.startswith("builtins.")
    assert False
