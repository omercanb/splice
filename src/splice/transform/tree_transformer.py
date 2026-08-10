"""
Base class for AST-level transforms
Also supports hoisting statements in the function or global scope
"""

from typing import TypeVar

from mypy.nodes import (
    AssertStmt,
    AssignmentExpr,
    AssignmentStmt,
    Block,
    CallExpr,
    CastExpr,
    ClassDef,
    ComparisonExpr,
    ConditionalExpr,
    Context,
    Decorator,
    DelStmt,
    DictExpr,
    DictionaryComprehension,
    Expression,
    ForStmt,
    FuncDef,
    GeneratorExpr,
    IfStmt,
    IndexExpr,
    LambdaExpr,
    ListComprehension,
    ListExpr,
    MemberExpr,
    MypyFile,
    Node,
    OperatorAssignmentStmt,
    OpExpr,
    RaiseStmt,
    ReturnStmt,
    SetComprehension,
    SetExpr,
    SliceExpr,
    StarExpr,
    Statement,
    TryStmt,
    TupleExpr,
    UnaryExpr,
    WhileStmt,
)

from splice.visitor import Visitor

_ContextT = TypeVar("_ContextT", bound=Context)


def copy_position(node: _ContextT, source: Context) -> _ContextT:
    """A freshly constructed node has no position (-1) until stamped with one."""
    node.line = source.line
    node.column = source.column
    node.end_line = source.end_line
    node.end_column = source.end_column
    return node


class Transformer(Visitor[Node]):
    def __init__(self):
        # One list per statement list currently open (MypyFile.defs, then one
        # more per nested Block). Index 0 is always MypyFile.defs, since
        # visit_mypy_file is the outermost _visit_statement_list call - that's
        # what lets hoist_global() reach module scope from any depth.
        self._hoist_stack: list[list[Statement]] = []

    def hoist(self, stmts: list[Statement]) -> None:
        """Splice statements in immediately before the statement being visited."""
        assert self._hoist_stack, "hoist() outside statement context"
        self._hoist_stack[-1].extend(stmts)

    def hoist_global(self, stmts: list[Statement]) -> None:
        """Add definitions at module scope, before the top-level definition
        currently being visited - for helpers a nested transform wants to
        introduce (e.g. lifting a comprehension out into its own function).
        """
        assert self._hoist_stack, "hoist_global() outside statement context"
        self._hoist_stack[0].extend(stmts)

    def _visit_statement_list(self, stmts: list[Statement]) -> list[Statement]:
        """Visit each statement, splicing in whatever it hoisted right before it."""
        self._hoist_stack.append([])
        new_body: list[Statement] = []
        for stmt in stmts:
            self._hoist_stack[-1] = []
            new_stmt = self.visit(stmt)
            new_body.extend(self._hoist_stack[-1])
            new_body.append(new_stmt)
        self._hoist_stack.pop()
        return new_body

    def visit_mypy_file(self, o: MypyFile) -> Node:
        o.defs = self._visit_statement_list(o.defs)
        return o

    def visit_block(self, o: Block) -> Node:
        o.body = self._visit_statement_list(o.body)
        return o

    def visit_func_def(self, o: FuncDef) -> Node:
        for argument in o.arguments:
            if argument.initializer is not None:
                argument.initializer = self.visit(argument.initializer)
        self.visit(o.body)
        return o

    def visit_class_def(self, o: ClassDef) -> Node:
        self.visit(o.defs)
        return o

    def visit_assignment_stmt(self, o: AssignmentStmt) -> Node:
        o.lvalues = [self.visit(lvalue) for lvalue in o.lvalues]
        o.rvalue = self.visit(o.rvalue)
        return o

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> Node:
        o.lvalue = self.visit(o.lvalue)
        o.rvalue = self.visit(o.rvalue)
        return o

    def visit_expression_stmt(self, o) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_return_stmt(self, o: ReturnStmt) -> Node:
        if o.expr is not None:
            o.expr = self.visit(o.expr)
        return o

    def visit_if_stmt(self, o: IfStmt) -> Node:
        o.expr = [self.visit(condition) for condition in o.expr]
        for body in o.body:
            self.visit(body)
        if o.else_body is not None:
            self.visit(o.else_body)
        return o

    def visit_while_stmt(self, o: WhileStmt) -> Node:
        o.expr = self.visit(o.expr)
        self.visit(o.body)
        if o.else_body is not None:
            self.visit(o.else_body)
        return o

    def visit_for_stmt(self, o: ForStmt) -> Node:
        o.index = self.visit(o.index)
        o.expr = self.visit(o.expr)
        self.visit(o.body)
        if o.else_body is not None:
            self.visit(o.else_body)
        return o

    def visit_try_stmt(self, o: TryStmt) -> Node:
        self.visit(o.body)
        o.types = [t if t is None else self.visit(t) for t in o.types]
        o.vars = [v if v is None else self.visit(v) for v in o.vars]
        for handler in o.handlers:
            self.visit(handler)
        if o.else_body is not None:
            self.visit(o.else_body)
        if o.finally_body is not None:
            self.visit(o.finally_body)
        return o

    def visit_assert_stmt(self, o: AssertStmt) -> Node:
        o.expr = self.visit(o.expr)
        if o.msg is not None:
            o.msg = self.visit(o.msg)
        return o

    def visit_del_stmt(self, o: DelStmt) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_raise_stmt(self, o: RaiseStmt) -> Node:
        if o.expr is not None:
            o.expr = self.visit(o.expr)
        if o.from_expr is not None:
            o.from_expr = self.visit(o.from_expr)
        return o

    def visit_break_stmt(self, o) -> Node:
        return o

    def visit_continue_stmt(self, o) -> Node:
        return o

    def visit_pass_stmt(self, o) -> Node:
        return o

    def visit_import(self, o) -> Node:
        return o

    def visit_import_from(self, o) -> Node:
        return o

    def visit_import_all(self, o) -> Node:
        return o

    def visit_decorator(self, o: Decorator) -> Node:
        o.func = self.visit(o.func)
        return o

    def visit_name_expr(self, o) -> Node:
        return o

    def visit_int_expr(self, o) -> Node:
        return o

    def visit_str_expr(self, o) -> Node:
        return o

    def visit_bytes_expr(self, o) -> Node:
        return o

    def visit_float_expr(self, o) -> Node:
        return o

    def visit_complex_expr(self, o) -> Node:
        return o

    def visit_ellipsis(self, o) -> Node:
        return o

    def visit_var(self, o) -> Node:
        return o

    def visit_temp_node(self, o) -> Node:
        return o

    def visit_member_expr(self, o: MemberExpr) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_call_expr(self, o: CallExpr) -> Node:
        o.callee = self.visit(o.callee)
        o.args = [self.visit(argument) for argument in o.args]
        return o

    def visit_op_expr(self, o: OpExpr) -> Node:
        o.left = self.visit(o.left)
        o.right = self.visit(o.right)
        return o

    def visit_comparison_expr(self, o: ComparisonExpr) -> Node:
        o.operands = [self.visit(operand) for operand in o.operands]
        return o

    def visit_unary_expr(self, o: UnaryExpr) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_index_expr(self, o: IndexExpr) -> Node:
        o.base = self.visit(o.base)
        o.index = self.visit(o.index)
        return o

    def visit_slice_expr(self, o: SliceExpr) -> Node:
        if o.begin_index is not None:
            o.begin_index = self.visit(o.begin_index)
        if o.end_index is not None:
            o.end_index = self.visit(o.end_index)
        if o.stride is not None:
            o.stride = self.visit(o.stride)
        return o

    def visit_list_expr(self, o: ListExpr) -> Node:
        o.items = [self.visit(item) for item in o.items]
        return o

    def visit_set_expr(self, o: SetExpr) -> Node:
        o.items = [self.visit(item) for item in o.items]
        return o

    def visit_tuple_expr(self, o: TupleExpr) -> Node:
        o.items = [self.visit(item) for item in o.items]
        return o

    def visit_dict_expr(self, o: DictExpr) -> Node:
        o.items = [
            (key if key is None else self.visit(key), self.visit(value))
            for key, value in o.items
        ]
        return o

    def visit_lambda_expr(self, o: LambdaExpr) -> Node:
        self.visit(o.body)
        return o

    def visit_conditional_expr(self, o: ConditionalExpr) -> Node:
        o.cond = self.visit(o.cond)
        o.if_expr = self.visit(o.if_expr)
        o.else_expr = self.visit(o.else_expr)
        return o

    def visit_star_expr(self, o: StarExpr) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_assignment_expr(self, o: AssignmentExpr) -> Node:
        o.target = self.visit(o.target)
        o.value = self.visit(o.value)
        return o

    def visit_cast_expr(self, o: CastExpr) -> Node:
        o.expr = self.visit(o.expr)
        return o

    def visit_generator_expr(self, o: GeneratorExpr) -> Node:
        self.visit_comprehension_parts(o)
        o.left_expr = self.visit(o.left_expr)
        return o

    def visit_list_comprehension(self, o: ListComprehension) -> Node:
        o.generator = self.visit(o.generator)
        return o

    def visit_set_comprehension(self, o: SetComprehension) -> Node:
        o.generator = self.visit(o.generator)
        return o

    def visit_dictionary_comprehension(self, o: DictionaryComprehension) -> Node:
        self.visit_comprehension_parts(o)
        o.key = self.visit(o.key)
        o.value = self.visit(o.value)
        return o

    def visit_comprehension_parts(self, o) -> None:
        """The `for index in sequence if condition` clauses every kind shares."""
        o.indices = [self.visit(index) for index in o.indices]
        o.sequences = [self.visit(sequence) for sequence in o.sequences]
        o.condlists = [
            [self.visit(condition) for condition in conditions] for conditions in o.condlists
        ]
