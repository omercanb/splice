"""
Transforms comprehensions into function calls
"""

from mypy.nodes import (
    ArgKind,
    Argument,
    AssignmentStmt,
    Block,
    CallExpr,
    Context,
    DictExpr,
    DictionaryComprehension,
    Expression,
    ExpressionStmt,
    ForStmt,
    FuncDef,
    GeneratorExpr,
    IfStmt,
    ListComprehension,
    ListExpr,
    MemberExpr,
    NameExpr,
    Node,
    ReturnStmt,
    SetComprehension,
    SetExpr,
    Statement,
    Var,
)
from mypy.types import CallableType, Type

from splice import pipeline
from splice.analysis.free_variables import get_free_variables
from splice.namer import TempNameGenerator
from splice.transform.tree_transformer import Transformer, copy_position


def method_expr(
    base: Expression,
    method_name: str,
    method_params: list[Expression],
    source: Context,
) -> CallExpr:
    method_arg_kinds = [ArgKind.ARG_POS for _ in method_params]
    method_arg_names = [None for _ in method_params]
    member = copy_position(MemberExpr(base, method_name), source)
    call = CallExpr(member, method_params, method_arg_kinds, method_arg_names)
    return copy_position(call, source)


def call_expr(
    function_name: str, function_params: list[Expression], source: Context
) -> CallExpr:
    function_arg_kinds = [ArgKind.ARG_POS for _ in function_params]
    function_arg_names = [None for _ in function_params]
    name = copy_position(NameExpr(function_name), source)
    call = CallExpr(name, function_params, function_arg_kinds, function_arg_names)
    return copy_position(call, source)


class ComprehensionRemover(Transformer):
    def __init__(self, types: dict[Expression, Type]):
        super().__init__()
        self.types = types
        self.transform_done = False

    def transform_comprehension(
        self,
        o: GeneratorExpr | DictionaryComprehension,
        comprehension_type: Type,
        new_name: str,
        empty_container: Expression,
        method_name: str,
        method_params: list[Expression],
    ):
        """Create a new funcdef that behaves like the comprehension in the global scope and return the function call that replaces the comprehension"""
        self.transform_done = True

        def typed_name(name: str) -> NameExpr:
            e = copy_position(NameExpr(name), o)
            self.types[e] = comprehension_type
            return e

        # Create a temp variable which will be returned
        tmp_name = TempNameGenerator().temp_name("tmp")
        declaration_name = typed_name(tmp_name)
        declaration_name.is_new_def = True

        copy_position(empty_container, o)
        initialize_tmp = copy_position(
            AssignmentStmt([declaration_name], empty_container), o
        )
        self.types[empty_container] = comprehension_type

        # Inner statement that grows the tmp variable
        grow_tmp_call = method_expr(typed_name(tmp_name), method_name, method_params, o)

        loop_body = self.expand_comprehension(
            o, copy_position(ExpressionStmt(grow_tmp_call), o)
        )

        # Return the temp variable
        return_tmp = copy_position(ReturnStmt(typed_name(tmp_name)), o)

        function_body = copy_position(
            Block([initialize_tmp, loop_body, return_tmp]), o
        )

        # Create a function with the appropriate type
        free_variables = get_free_variables(o)
        types_of_free_variables = [self.types[var] for var in free_variables]
        # Find the free variables to capture

        args = [
            Argument(Var(v.name, t), t, None, ArgKind.ARG_POS)
            for v, t in zip(free_variables, types_of_free_variables)
        ]
        for arg in args:
            arg.variable.type = arg.type_annotation

        func_type = CallableType(
            arg_types=types_of_free_variables,
            arg_kinds=[ArgKind.ARG_POS for _ in args],
            arg_names=[v.name for v in free_variables],
            ret_type=comprehension_type,
            fallback=pipeline.function_fallback,
        )

        function_definition = copy_position(
            FuncDef(new_name, args, function_body, func_type), o
        )
        self.hoist_global([function_definition])

        replacing_call_arguments = [
            copy_position(NameExpr(free_var.name), o) for free_var in free_variables
        ]
        replacing_call = call_expr(new_name, replacing_call_arguments, o)

        return replacing_call

    def expand_comprehension(
        self, o: GeneratorExpr | DictionaryComprehension, body_stmt: Statement
    ) -> Statement:
        """
        Transform a comprehension into a for loop and insert the body statement
        [(i, j) for i in range(10) if i % 2 for j in range(10) if j % 2] ->
        for i in range(10):
            if i % 2:
                for j in range(10):
                    if j % 2:
                        body_stmt
        """
        inner = body_stmt
        for index_variable, iterated_sequence, conditions in zip(
            reversed(o.indices), reversed(o.sequences), reversed(o.condlists)
        ):
            for condition in conditions:
                inner = copy_position(
                    IfStmt(
                        [condition],
                        [copy_position(Block([inner]), o)],
                        else_body=None,
                    ),
                    o,
                )
            # for index_variable in iterated_sequence: if condition[0]: if condition[1]: ...
            inner = copy_position(
                ForStmt(
                    index_variable,
                    iterated_sequence,
                    copy_position(Block([inner]), o),
                    else_body=None,
                ),
                o,
            )
        return inner

    def visit_set_comprehension(self, o: SetComprehension):
        new_name = TempNameGenerator().temp_name("set_comprehension")
        return self.transform_comprehension(
            o.generator,
            self.types[o],
            new_name,
            SetExpr([]),
            "add",
            [o.generator.left_expr],
        )

    def visit_list_comprehension(self, o: ListComprehension):
        new_name = TempNameGenerator().temp_name("list_comprehension")
        return self.transform_comprehension(
            o.generator,
            self.types[o],
            new_name,
            ListExpr([]),
            "append",
            [o.generator.left_expr],
        )

    def visit_dictionary_comprehension(self, o: DictionaryComprehension):
        new_name = TempNameGenerator().temp_name("dict_comprehension")
        return self.transform_comprehension(
            o, self.types[o], new_name, DictExpr([]), "__setitem__", [o.key, o.value]
        )


def apply_comprehension_transforms(tree: Node, types):
    while True:
        t = ComprehensionRemover(types)
        t.visit(tree)
        if not t.transform_done:
            break
