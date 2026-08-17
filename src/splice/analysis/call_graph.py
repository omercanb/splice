"""Build a call graph"""

# Walk the whole tree
# Create a graph of funcdefs to funcdefs with parameter bindings as the edges


from dataclasses import dataclass, field
from typing import Optional

from mypy.nodes import (
    CallExpr,
    Expression,
    FuncDef,
    MemberExpr,
    NameExpr,
    TypeInfo,
    Var,
)
from mypy.types import Instance, get_proper_type

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
    def __init__(self, types: TypeTable):
        self.current_function: Optional[FuncDef] = None
        self.types = types

    def visit_func_def(self, o: FuncDef):
        self.current_function = o
        super().visit_func_def(o)
        self.current_function = None

    def visit_call_expr(self, o: CallExpr):
        if is_call_builtin(o, self.types):
            super().visit_call_expr(o)
            return

        param_bindings = []

        # If current function add edge to call graph
        # Note: The function needs to be a node in the graph
        return super().visit_call_expr(o)


def match_call_arguments(o: CallExpr, types: TypeTable) -> list[tuple[Expression, Var]]:
    funcdef = resolve_funcdef(o, types)
    params = [arg.variable for arg in funcdef.arguments]

    if isinstance(o.callee, MemberExpr):  # Method Call
        self_binding = (o.callee.expr, params[0])
        return [self_binding] + list(zip(o.args, params[1:]))

    if isinstance(o.callee, NameExpr) and isinstance(o.callee.node, TypeInfo):
        # Constructor Call
        params = params[1:]

    return list(zip(o.args, params))


def resolve_funcdef(o: CallExpr, types: TypeTable) -> FuncDef:
    assert not is_call_builtin(o, types)

    if isinstance(o.callee, NameExpr):
        node = o.callee.node
        if isinstance(node, FuncDef):
            return node

        assert isinstance(node, TypeInfo)
        init = node.get("__init__")
        assert init is not None and isinstance(init.node, FuncDef)
        return init.node

    assert isinstance(o.callee, MemberExpr)
    receiver_type = get_proper_type(types[o.callee.expr])
    assert isinstance(receiver_type, Instance)
    method = receiver_type.type.get(o.callee.name)
    assert method is not None and isinstance(method.node, FuncDef)
    return method.node


def is_call_builtin(o: CallExpr, types: TypeTable):
    if isinstance(o.callee, NameExpr):
        return o.callee.fullname.startswith("builtins.")
    if isinstance(o.callee, MemberExpr):
        t = types[o.callee.expr]
        assert isinstance(t, Instance)
        return t.type.fullname.startswith("builtins.")
    assert False
