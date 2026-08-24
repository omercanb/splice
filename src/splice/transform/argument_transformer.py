"""Rewrites a call to a user-defined function, its constructor, or a builtin like print/sorted"""

# Because C++ has no way to pass in only a middle keyword argument, all default arguments have to be passed in at the call site.

from mypy.nodes import (
    ARG_POS,
    ARG_STAR,
    ARG_STAR2,
    CallExpr,
    Expression,
    NameExpr,
    Node,
    TupleExpr,
    TypeInfo,
)

from splice.analysis.call_graph import is_call_splice_intrinsic, resolve_funcdef
from splice.ast_utils import TypeTable
from splice.codegen.builtins import BUILTIN_SIGNATURES, BuiltinParam
from splice.transform.tree_transformer import Transformer, copy_position


class ArgumentTransformer(Transformer):
    def __init__(self, types: TypeTable):
        super().__init__()
        self.types = types

    def visit_call_expr(self, o: CallExpr) -> Node:
        o.args = [self.visit(argument) for argument in o.args]
        self.reorder_arguments(o)
        return o

    def reorder_arguments(self, o: CallExpr) -> None:
        if not isinstance(o.callee, NameExpr):
            return
        if is_call_splice_intrinsic(o):
            return
        if any(kind in (ARG_STAR, ARG_STAR2) for kind in o.arg_kinds):
            return  # *args/**kwargs at the call site aren't supported

        signature = BUILTIN_SIGNATURES.get(o.callee.fullname)
        if signature is not None:
            self._rewrite_call(o, signature.params, variadic=signature.variadic)
            return
        if o.callee.fullname.startswith("builtins."):
            return  # plain positional builtin, nothing to fill in

        funcdef = resolve_funcdef(o, self.types)
        if funcdef is None:
            return
        params = funcdef.arguments
        if isinstance(o.callee.node, TypeInfo):
            params = params[1:]  # a constructor call never passes self
        self._rewrite_call(
            o,
            [BuiltinParam(p.variable.name, p.initializer) for p in params],
            variadic=False,
        )

    def _rewrite_call(
        self, o: CallExpr, params: list[BuiltinParam], variadic: bool
    ) -> None:
        by_name = {
            name: arg for arg, name in zip(o.args, o.arg_names) if name is not None
        }
        positional = iter(arg for arg, name in zip(o.args, o.arg_names) if name is None)

        new_args: list[Expression] = []
        if variadic:
            new_args.append(copy_position(TupleExpr(list(positional)), o))
            positional = iter(())

        for param in params:
            if param.name in by_name:
                new_args.append(by_name[param.name])
                continue
            positional_arg = next(positional, None)
            if positional_arg is not None:
                new_args.append(positional_arg)
            else:
                assert param.default is not None
                new_args.append(param.default)

        o.args = new_args
        o.arg_names = [None] * len(new_args)
        o.arg_kinds = [ARG_POS] * len(new_args)
