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
    OverloadedFuncDef,
    TypeInfo,
    Var,
)
from mypy.types import Instance, get_proper_type

from splice.ast_utils import TypeTable
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
        if not is_call_builtin(o, self.types) and not is_call_splice_intrinsic(o):
            callee = resolve_funcdef(o, self.types)
            if callee is not None:
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
    if funcdef is None:
        # No user-defined __init__
        return []
    params = [arg.variable for arg in funcdef.arguments]

    if isinstance(o.callee, MemberExpr):
        # Method Call
        self_binding = (o.callee.expr, params[0])
        return [self_binding] + list(zip(o.args, params[1:]))

    if isinstance(o.callee, NameExpr) and isinstance(o.callee.node, TypeInfo):
        # Constructor Call (remove the self param)
        params = params[1:]

    return list(zip(o.args, params))


def _usable_funcdef(node) -> Optional[FuncDef]:
    """A FuncDef with a real argument list, or None if it doesn't have one.

    Two cases have no argument list: an overloaded function, and a
    stub-only FuncDef (eg. IO.read, from a .pyi file). We check by trying
    to read .arguments instead of checking the node's type, since that's
    the only way that covers both cases.
    """
    if not isinstance(node, FuncDef) or isinstance(node, OverloadedFuncDef):
        return None
    if getattr(node, "arguments", None) is None:
        return None
    return node


def resolve_funcdef(o: CallExpr, types: TypeTable) -> Optional[FuncDef]:
    """The function being called, or None if there's no argument list to
    bind against. This happens for a class with no __init__ (its
    constructor takes zero arguments), or for anything _usable_funcdef
    rejects.
    """
    assert not is_call_builtin(o, types)

    if isinstance(o.callee, NameExpr):
        node = o.callee.node
        if isinstance(node, FuncDef):
            return _usable_funcdef(node)

        assert isinstance(node, TypeInfo)
        init = node.get("__init__")
        if init is None:
            return None
        resolved = _usable_funcdef(init.node)
        if resolved is not None and resolved.fullname.startswith("builtins."):
            return None
        return resolved

    assert isinstance(o.callee, MemberExpr)
    receiver_type = get_proper_type(types[o.callee.expr])
    assert isinstance(receiver_type, Instance)
    method = receiver_type.type.get(o.callee.name)
    if method is None:
        return None
    return _usable_funcdef(method.node)


def is_call_builtin(o: CallExpr, types: TypeTable):
    if isinstance(o.callee, NameExpr):
        return o.callee.fullname.startswith("builtins.")
    if isinstance(o.callee, MemberExpr):
        t = types[o.callee.expr]
        assert isinstance(t, Instance)
        return t.type.fullname.startswith("builtins.")
    assert False


def is_call_splice_intrinsic(o: CallExpr) -> bool:
    """A call to something defined in splice.stdlib (eg. copy()). These
    have a real Python body for running under plain CPython, but are never
    actually compiled from it - codegen emits a hand-written C++
    implementation instead, the same way a real builtin does.
    """
    return isinstance(o.callee, NameExpr) and o.callee.fullname.startswith(
        "splice.stdlib."
    )
