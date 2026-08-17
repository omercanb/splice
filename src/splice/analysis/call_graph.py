"""Build a call graph"""

from dataclasses import dataclass, field
from typing import Optional

from mypy.nodes import (
    CallExpr,
    Expression,
    FuncDef,
    MemberExpr,
    MypyFile,
    NameExpr,
    TypeInfo,
    Var,
)
from mypy.types import Instance, get_proper_type

from splice.pipeline import TypeTable
from splice.visitor import Traverser


@dataclass
class CallEdge:
    call: CallExpr
    caller: FuncDef
    callee: FuncDef
    bindings: list[tuple[Expression, Var]] = field(default_factory=list)


@dataclass
class CallGraph:
    edges: list[CallEdge] = field(default_factory=list)


class CallGraphProducer(Traverser):
    def __init__(self, types: TypeTable):
        self.current_function: Optional[FuncDef] = None
        self.types = types
        self.call_graph = CallGraph()

    def visit_func_def(self, o: FuncDef):
        self.current_function = o
        super().visit_func_def(o)
        self.current_function = None

    def visit_call_expr(self, o: CallExpr):
        if not is_call_builtin(o, self.types):
            callee = resolve_funcdef(o, self.types)
            bindings = match_call_arguments(o, self.types)
            assert self.current_function
            edge = CallEdge(o, self.current_function, callee, bindings)
            self.call_graph.edges.append(edge)

        super().visit_call_expr(o)


def compute_call_graph(tree: MypyFile, types: TypeTable):
    visitor = CallGraphProducer(types)
    visitor.visit(tree)
    return visitor.call_graph


def match_call_arguments(o: CallExpr, types: TypeTable) -> list[tuple[Expression, Var]]:
    funcdef = resolve_funcdef(o, types)
    params = [arg.variable for arg in funcdef.arguments]

    if isinstance(o.callee, MemberExpr):
        # Method Call
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
        assert (
            init is not None
            and isinstance(init.node, FuncDef)
            and not init.node.fullname.startswith("builtins.")
        ), "every class must define __init__"
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
