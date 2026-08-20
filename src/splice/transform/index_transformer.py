"""Rewrites subscript reads and writes into explicit dunder-method calls.

`a[i]` becomes `a.__getitem__(i)`, `a[i] = v` becomes `a.__setitem__(i, v)`,
and a slice index `a[i:j:k]` becomes `a.__getitem__(slice(i, j, k))`, with a
missing bound passed as `None`.
For `-1` specifically it stays a -1 here but turns into back() at codegen time
"""

from mypy.nodes import (
    ArgKind,
    AssignmentStmt,
    CallExpr,
    Context,
    Expression,
    ExpressionStmt,
    IndexExpr,
    MemberExpr,
    NameExpr,
    Node,
    OperatorAssignmentStmt,
    SliceExpr,
)
from mypy.types import TupleType, Type, get_proper_type

from splice.transform.tree_transformer import Transformer, copy_position


def _none_expr(source: Context) -> NameExpr:
    none = NameExpr("None")
    none.fullname = "builtins.None"
    return copy_position(none, source)


def _method_call(
    base: Expression, name: str, args: list[Expression], source: Context
) -> CallExpr:
    member = copy_position(MemberExpr(base, name), source)
    call = CallExpr(member, args, [ArgKind.ARG_POS for _ in args], [None] * len(args))
    return copy_position(call, source)


class IndexTransformer(Transformer):
    def __init__(self, types: dict[Expression, Type]):
        super().__init__()
        self.types = types

    def _is_tuple(self, base: Expression) -> bool:
        return isinstance(get_proper_type(self.types.get(base)), TupleType)

    def _slice_call(self, o: SliceExpr) -> CallExpr:
        bounds = [
            self.visit(bound) if bound is not None else _none_expr(o)
            for bound in (o.begin_index, o.end_index, o.stride)
        ]
        name = copy_position(NameExpr("slice"), o)
        name.fullname = "builtins.slice"
        call = CallExpr(name, bounds, [ArgKind.ARG_POS] * 3, [None] * 3)
        return copy_position(call, o)

    def visit_index_expr(self, o: IndexExpr) -> Node:
        # A subscript in type position (eg. `Matrix = List[List[float]]`)
        # looks identical to a real one syntactically, but mypy never
        # type-checks it as a value, so it has no entry in self.types.
        # Leave it exactly as-is - it isn't a runtime operation to lower.
        if o not in self.types:
            return o
        # Special case for Array[T, N]
        if isinstance(o.base, NameExpr) and o.base.fullname == "splice.stdlib.Array":
            return o
        o.base = self.visit(o.base)
        if self._is_tuple(o.base):
            o.index = self.visit(o.index)
            return o
        if isinstance(o.index, SliceExpr):
            result = _method_call(o.base, "__getitem__", [self._slice_call(o.index)], o)
        else:
            result = _method_call(o.base, "__getitem__", [self.visit(o.index)], o)
        self.types[result] = self.types[o]
        return result

    def visit_assignment_stmt(self, o: AssignmentStmt) -> Node:
        o.rvalue = self.visit(o.rvalue)
        if len(o.lvalues) == 1 and isinstance(o.lvalues[0], IndexExpr):
            target = o.lvalues[0]
            target.base = self.visit(target.base)
            if not self._is_tuple(target.base):
                index = self.visit(target.index)
                call = _method_call(target.base, "__setitem__", [index, o.rvalue], o)
                return copy_position(ExpressionStmt(call), o)
            target.index = self.visit(target.index)
            return o
        o.lvalues = [self.visit(lvalue) for lvalue in o.lvalues]
        return o

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> Node:
        # a[i] += v needs a reference to mutate in place, not __setitem__
        # (which takes the whole new value at once) - __getitem__ already
        # returns one, same as for a plain read.
        o.rvalue = self.visit(o.rvalue)
        if isinstance(o.lvalue, IndexExpr):
            target = o.lvalue
            target.base = self.visit(target.base)
            if not self._is_tuple(target.base):
                index = self.visit(target.index)
                o.lvalue = _method_call(target.base, "__getitem__", [index], target)
            else:
                target.index = self.visit(target.index)
            return o
        o.lvalue = self.visit(o.lvalue)
        return o
