"""
Renders a mypy AST back to Python source, for debugging transforms.
Also includes type annotations.
"""

from mypy.nodes import (
    ARG_STAR,
    ARG_STAR2,
    AssertStmt,
    AssignmentExpr,
    AssignmentStmt,
    Block,
    CallExpr,
    CastExpr,
    ClassDef,
    ComparisonExpr,
    ConditionalExpr,
    Decorator,
    DelStmt,
    DictExpr,
    DictionaryComprehension,
    ForStmt,
    FuncDef,
    GeneratorExpr,
    IfStmt,
    Import,
    ImportAll,
    ImportFrom,
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
    TempNode,
    TryStmt,
    TupleExpr,
    UnaryExpr,
    WhileStmt,
)
from mypy.types import CallableType, get_proper_type

from splice.visitor import Visitor


class PythonPrinter(Visitor[str]):
    """Turns a mypy AST back into Python source text, annotations included."""

    def visit_mypy_file(self, o: MypyFile) -> str:
        return "\n".join(self.visit(d) for d in o.defs)

    def _block(self, o: Block) -> str:
        lines = "\n".join(self.visit(stmt) for stmt in o.body) if o.body else "pass"
        return "\n".join("    " + line for line in lines.splitlines())

    # -- definitions --

    def visit_func_def(self, o: FuncDef) -> str:
        # o.type (once analyzed) carries the resolved arg/return types; each
        # Argument's own type_annotation is the pre-analysis annotation and
        # prints with a trailing "?" (mypy's marker for an unbound type), so
        # prefer the resolved ones and only fall back if the shapes disagree
        # (e.g. a bound method's CallableType may omit the `self` argument).
        proper = get_proper_type(o.type) if o.type is not None else None
        arg_types = (
            proper.arg_types
            if isinstance(proper, CallableType)
            and len(proper.arg_types) == len(o.arguments)
            else None
        )
        params = ", ".join(
            self._param(a, arg_types[i] if arg_types is not None else None)
            for i, a in enumerate(o.arguments)
        )
        return_annotation = (
            f" -> {proper.ret_type}" if isinstance(proper, CallableType) else ""
        )
        header = f"def {o.name}({params}){return_annotation}:"
        return f"{header}\n{self._block(o.body)}"

    def _param(self, argument, resolved_type) -> str:
        prefix = {ARG_STAR: "*", ARG_STAR2: "**"}.get(argument.kind, "")
        text = f"{prefix}{argument.variable.name}"
        annotation = (
            resolved_type if resolved_type is not None else argument.type_annotation
        )
        if annotation is not None:
            text += f": {annotation}"
        if argument.initializer is not None:
            text += f" = {self.visit(argument.initializer)}"
        return text

    def visit_class_def(self, o: ClassDef) -> str:
        bases = ", ".join(self.visit(base) for base in o.base_type_exprs)
        header = f"class {o.name}({bases}):" if bases else f"class {o.name}:"
        return f"{header}\n{self._block(o.defs)}"

    def visit_decorator(self, o: Decorator) -> str:
        decorators = "".join(f"@{self.visit(d)}\n" for d in o.decorators)
        return f"{decorators}{self.visit(o.func)}"

    # -- statements --

    def visit_assignment_stmt(self, o: AssignmentStmt) -> str:
        targets = " = ".join(self.visit(lvalue) for lvalue in o.lvalues)
        annotation = f": {o.type}" if o.type is not None and o.new_syntax else ""
        if isinstance(o.rvalue, TempNode):
            return f"{targets}{annotation}"
        return f"{targets}{annotation} = {self.visit(o.rvalue)}"

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> str:
        return f"{self.visit(o.lvalue)} {o.op}= {self.visit(o.rvalue)}"

    def visit_expression_stmt(self, o) -> str:
        return self.visit(o.expr)

    def visit_return_stmt(self, o: ReturnStmt) -> str:
        if o.expr is None:
            return "return"
        return f"return {self.visit(o.expr)}"

    def visit_if_stmt(self, o: IfStmt) -> str:
        return self._if_clauses(o, "if")

    def _if_clauses(self, o: IfStmt, leading: str) -> str:
        parts = [f"{leading} {self.visit(o.expr[0])}:\n{self._block(o.body[0])}"]
        for condition, body in zip(o.expr[1:], o.body[1:]):
            parts.append(f"elif {self.visit(condition)}:\n{self._block(body)}")
        if o.else_body is not None:
            # elif chains parse as a single nested IfStmt in else_body - unwrap
            # it back into an `elif` clause instead of `else:\n    if ...:`.
            if len(o.else_body.body) == 1 and isinstance(o.else_body.body[0], IfStmt):
                parts.append(self._if_clauses(o.else_body.body[0], "elif"))
            else:
                parts.append(f"else:\n{self._block(o.else_body)}")
        return "\n".join(parts)

    def visit_while_stmt(self, o: WhileStmt) -> str:
        text = f"while {self.visit(o.expr)}:\n{self._block(o.body)}"
        if o.else_body is not None:
            text += f"\nelse:\n{self._block(o.else_body)}"
        return text

    def visit_for_stmt(self, o: ForStmt) -> str:
        prefix = "async " if o.is_async else ""
        text = (
            f"{prefix}for {self.visit(o.index)} in {self.visit(o.expr)}:\n"
            f"{self._block(o.body)}"
        )
        if o.else_body is not None:
            text += f"\nelse:\n{self._block(o.else_body)}"
        return text

    def visit_try_stmt(self, o: TryStmt) -> str:
        parts = [f"try:\n{self._block(o.body)}"]
        for exc_type, exc_var, handler in zip(o.types, o.vars, o.handlers):
            header = "except"
            if exc_type is not None:
                header += f" {self.visit(exc_type)}"
            if exc_var is not None:
                header += f" as {self.visit(exc_var)}"
            parts.append(f"{header}:\n{self._block(handler)}")
        if o.else_body is not None:
            parts.append(f"else:\n{self._block(o.else_body)}")
        if o.finally_body is not None:
            parts.append(f"finally:\n{self._block(o.finally_body)}")
        return "\n".join(parts)

    def visit_assert_stmt(self, o: AssertStmt) -> str:
        text = f"assert {self.visit(o.expr)}"
        if o.msg is not None:
            text += f", {self.visit(o.msg)}"
        return text

    def visit_del_stmt(self, o: DelStmt) -> str:
        return f"del {self.visit(o.expr)}"

    def visit_raise_stmt(self, o: RaiseStmt) -> str:
        if o.expr is None:
            return "raise"
        text = f"raise {self.visit(o.expr)}"
        if o.from_expr is not None:
            text += f" from {self.visit(o.from_expr)}"
        return text

    def visit_break_stmt(self, o) -> str:
        return "break"

    def visit_continue_stmt(self, o) -> str:
        return "continue"

    def visit_pass_stmt(self, o) -> str:
        return "pass"

    def visit_import(self, o: Import) -> str:
        names = ", ".join(
            name if alias is None else f"{name} as {alias}" for name, alias in o.ids
        )
        return f"import {names}"

    def visit_import_from(self, o: ImportFrom) -> str:
        names = ", ".join(
            name if alias is None else f"{name} as {alias}" for name, alias in o.names
        )
        return f"from {'.' * o.relative}{o.id} import {names}"

    def visit_import_all(self, o: ImportAll) -> str:
        return f"from {'.' * o.relative}{o.id} import *"

    # -- leaf expressions --

    def visit_name_expr(self, o) -> str:
        return o.name

    def visit_int_expr(self, o) -> str:
        return str(o.value)

    def visit_str_expr(self, o) -> str:
        return repr(o.value)

    def visit_bytes_expr(self, o) -> str:
        return repr(o.value.encode())

    def visit_float_expr(self, o) -> str:
        return repr(o.value)

    def visit_complex_expr(self, o) -> str:
        return repr(o.value)

    def visit_ellipsis(self, o) -> str:
        return "..."

    def visit_var(self, o) -> str:
        return o.name

    def visit_temp_node(self, o) -> str:
        return "<temp>"

    # -- compound expressions --

    def visit_member_expr(self, o: MemberExpr) -> str:
        return f"{self.visit(o.expr)}.{o.name}"

    def visit_call_expr(self, o: CallExpr) -> str:
        args = []
        for arg, kind, name in zip(o.args, o.arg_kinds, o.arg_names):
            text = self.visit(arg)
            if kind == ARG_STAR:
                text = f"*{text}"
            elif kind == ARG_STAR2:
                text = f"**{text}"
            elif name is not None:
                text = f"{name}={text}"
            args.append(text)
        return f"{self.visit(o.callee)}({', '.join(args)})"

    def visit_op_expr(self, o: OpExpr) -> str:
        return f"({self.visit(o.left)} {o.op} {self.visit(o.right)})"

    def visit_comparison_expr(self, o: ComparisonExpr) -> str:
        text = self.visit(o.operands[0])
        for op, operand in zip(o.operators, o.operands[1:]):
            text += f" {op} {self.visit(operand)}"
        return text

    def visit_unary_expr(self, o: UnaryExpr) -> str:
        return f"{o.op}{self.visit(o.expr)}"

    def visit_index_expr(self, o: IndexExpr) -> str:
        # A multi-arg subscript's index is a TupleExpr; visiting it plain would add spurious parens, e.g. Array[(int, N)].
        if isinstance(o.index, TupleExpr):
            index = ", ".join(self.visit(item) for item in o.index.items)
        else:
            index = self.visit(o.index)
        return f"{self.visit(o.base)}[{index}]"

    def visit_slice_expr(self, o: SliceExpr) -> str:
        begin = self.visit(o.begin_index) if o.begin_index is not None else ""
        end = self.visit(o.end_index) if o.end_index is not None else ""
        text = f"{begin}:{end}"
        if o.stride is not None:
            text += f":{self.visit(o.stride)}"
        return text

    def visit_list_expr(self, o: ListExpr) -> str:
        return f"[{', '.join(self.visit(item) for item in o.items)}]"

    def visit_set_expr(self, o: SetExpr) -> str:
        if not o.items:
            return "set()"
        return f"{{{', '.join(self.visit(item) for item in o.items)}}}"

    def visit_tuple_expr(self, o: TupleExpr) -> str:
        items = [self.visit(item) for item in o.items]
        if len(items) == 1:
            return f"({items[0]},)"
        return f"({', '.join(items)})"

    def visit_dict_expr(self, o: DictExpr) -> str:
        parts = []
        for key, value in o.items:
            if key is None:
                parts.append(f"**{self.visit(value)}")
            else:
                parts.append(f"{self.visit(key)}: {self.visit(value)}")
        return f"{{{', '.join(parts)}}}"

    def visit_lambda_expr(self, o: LambdaExpr) -> str:
        params = ", ".join(a.variable.name for a in o.arguments)
        return f"lambda {params}: {self.visit(o.expr())}"

    def visit_conditional_expr(self, o: ConditionalExpr) -> str:
        return (
            f"({self.visit(o.if_expr)} if {self.visit(o.cond)} "
            f"else {self.visit(o.else_expr)})"
        )

    def visit_star_expr(self, o: StarExpr) -> str:
        return f"*{self.visit(o.expr)}"

    def visit_assignment_expr(self, o: AssignmentExpr) -> str:
        return f"({self.visit(o.target)} := {self.visit(o.value)})"

    def visit_cast_expr(self, o: CastExpr) -> str:
        return f"cast({o.type}, {self.visit(o.expr)})"

    # -- comprehensions --

    def _comprehension_clauses(self, o) -> str:
        clauses = []
        for index, sequence, conditions, is_async in zip(
            o.indices, o.sequences, o.condlists, o.is_async
        ):
            clause = (
                f"{'async ' if is_async else ''}for {self.visit(index)} "
                f"in {self.visit(sequence)}"
            )
            for condition in conditions:
                clause += f" if {self.visit(condition)}"
            clauses.append(clause)
        return " ".join(clauses)

    def visit_generator_expr(self, o: GeneratorExpr) -> str:
        return f"({self.visit(o.left_expr)} {self._comprehension_clauses(o)})"

    def visit_list_comprehension(self, o: ListComprehension) -> str:
        g = o.generator
        return f"[{self.visit(g.left_expr)} {self._comprehension_clauses(g)}]"

    def visit_set_comprehension(self, o: SetComprehension) -> str:
        g = o.generator
        return f"{{{self.visit(g.left_expr)} {self._comprehension_clauses(g)}}}"

    def visit_dictionary_comprehension(self, o: DictionaryComprehension) -> str:
        return (
            f"{{{self.visit(o.key)}: {self.visit(o.value)} "
            f"{self._comprehension_clauses(o)}}}"
        )


def convert_to_python(node: Node) -> str:
    return PythonPrinter().visit(node)
