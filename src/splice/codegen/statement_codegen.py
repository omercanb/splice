"""Main code generation module"""

from mypy.nodes import (
    AssertStmt,
    AssignmentStmt,
    Block,
    BreakStmt,
    ClassDef,
    ContinueStmt,
    Decorator,
)
from mypy.nodes import Expression
from mypy.nodes import Expression as MypyExpression
from mypy.nodes import (
    ExpressionStmt,
    ForStmt,
    FuncDef,
    IfStmt,
    MypyFile,
    NameExpr,
    OperatorAssignmentStmt,
    RaiseStmt,
    ReturnStmt,
    TryStmt,
    Var,
    WhileStmt,
)
from mypy.types import Type

from splice.analysis.find_declarations import get_declarations
from splice.analysis.hotpath import collect_hotpath_funcs
from splice.analysis.mutation import MutationTable
from splice.codegen.class_def import write_class_bodies, write_class_declaration
from splice.codegen.exceptions import translate_raise_stmt, translate_try_stmt
from splice.codegen.expression_codegen import ExpressionCodegen
from splice.codegen.for_loop import translate_for_stmt
from splice.codegen.translation_utils import translate_func_signature
from splice.codegen.typegen import cpp_type
from splice.visitor import Traverser

class StatementCodegen(Traverser):
    """Generate C++ code from mypy AST statements."""

    def __init__(
        self,
        tree: MypyFile,
        types_dict: dict[MypyExpression, Type],
        mutations: MutationTable,
        extra_includes: list[str] | None = None,
    ):
        self.tree = tree
        self.types = types_dict
        self.mutations = mutations
        self.hotpath_funcs = collect_hotpath_funcs(tree)
        self.extra_includes = extra_includes or []
        self.expr_codegen = ExpressionCodegen(types_dict)
        self.indent_level = 0
        self.output: list[str] = []
        # Set while emitting globals, so visit_assignment_stmt declares-with-init instead of assigning into an already-declared name.
        self.constexpr_mode = False

    def visit_statements(self, statements):
        for statement in statements:
            self.visit(statement)

    def indent(self):
        self.indent_level += 1

    def unindent(self):
        self.indent_level -= 1

    def indented(self) -> str:
        """Return indentation string."""
        return "    " * self.indent_level

    def emit(self, code: str):
        """Emit a line of code."""
        # A blank separator stays blank rather than carrying the indent.
        self.output.append(f"{self.indented()}{code}" if code else "")

    def visit_block(self, o: Block):
        """Generate code for a block of statements."""
        self.indent()
        super().visit_block(o)
        self.unindent()

    def get_expr(self, expr: Expression, lvalue=False):
        self.expr_codegen.lvalue = lvalue
        return self.expr_codegen.visit(expr)

    def get_condition(self, expr: Expression) -> str:
        """Generate an expression to be used as a conditon"""
        self.expr_codegen.lvalue = False
        return self.expr_codegen.condition(expr)

    def translate_declaration(self, name: str, typ: Type):
        cpp = cpp_type(typ)
        return f"{cpp} {name};"

    def generate_includes(self):
        self.emit('#include "runtime.h"')
        for include in self.extra_includes:
            self.emit(f'#include "{include}"')
        self.emit(f"using namespace py;")

    def generate_global_declarations(self):
        """Globals are scalars only (check_mutable_value_semantics.py enforces this), so each compiles straight to a constexpr."""
        self.constexpr_mode = True
        for stmt in self.tree.defs:
            if not isinstance(stmt, AssignmentStmt) or stmt.is_alias_def:
                continue
            self.visit(stmt)
        self.constexpr_mode = False
        self.emit("")

    def generate(self) -> str:
        """Generate all C++ code."""
        self.generate_includes()
        self.generate_global_declarations()
        self.visit(self.tree)
        return "\n".join(self.output)

    def visit_mypy_file(self, o: MypyFile):
        """
        Emit declarations before bodies, so forward references compile.
        The ordering to be able to compile everything is
          1. forward-declare every class
          2. forward-declare every free function's signature
          3. emit each class's full structure, methods as signatures only
          4. emit every method body out-of-line (all classes now fully known)
          5. emit every free function's body
        """
        classes = [d for d in o.defs if isinstance(d, ClassDef)]
        # A decorator's identity is dropped, except @hotpath's effect on the signature (self.hotpath_funcs).
        functions = [
            d.func if isinstance(d, Decorator) else d
            for d in o.defs
            if isinstance(d, (FuncDef, Decorator))
        ]

        for class_def in classes:
            self.emit(f"class {class_def.name};")
        if classes:
            self.emit("")

        # A default like `def f(x: Other = Other())` would need Other's full
        # definition here, not just a forward declaration - a known gap.
        for function in functions:
            signature = translate_func_signature(
                function,
                self.expr_codegen,
                mutations=self.mutations,
                flatten=function in self.hotpath_funcs,
            )
            self.emit(f"{signature};")
        self.emit("")

        for class_def in classes:
            write_class_declaration(self, class_def)

        for class_def in classes:
            write_class_bodies(self, class_def)

        for function in functions:
            self.visit_func_def(function)

    def local_names(self, definition) -> set[str]:
        """The names a definition holds itself, rather than reading globally."""
        if not isinstance(definition, FuncDef):
            return set()
        declarations = get_declarations(definition, self.types)
        return set(declarations) | {
            argument.variable.name for argument in definition.arguments
        }

    def emit_function_body(self, header: str, o: FuncDef) -> None:
        """Emit a function/method body under an arbitrary header line.

        Shared between an inline definition (`visit_func_def`) and a class
        method's out-of-line definition, `ClassName::method(...) { ... }`
        (see class_def.py's `write_class_bodies`).
        """
        declarations = get_declarations(o, self.types)
        declaration_lines = [
            self.translate_declaration(name, typ) for name, typ in declarations.items()
        ]

        self.emit(header)
        self.indent()
        for declaration in declaration_lines:
            self.emit(declaration)
        for stmt in o.body.body:
            self.visit(stmt)
        self.unindent()
        self.emit("}")
        self.emit("")

    def visit_func_def(self, o: FuncDef):
        """Generate a function or method definition"""
        signature = translate_func_signature(
            o,
            self.expr_codegen,
            mutations=self.mutations,
            flatten=o in self.hotpath_funcs,
        )
        self.emit_function_body(f"{signature} {{", o)

    def visit_assignment_stmt(self, o: AssignmentStmt):
        # a[i] = x / a[-1] = x are already __setitem__/back() CallExprs by
        # the time this runs - IndexTransformer rewrote them before
        # codegen ever sees the tree, so lvalues[0] is a plain target here.
        target = o.lvalues[0]
        rhs = self.get_expr(o.rvalue)
        lhs = self.get_expr(target, lvalue=True)
        if self.constexpr_mode:
            assert isinstance(target, NameExpr) and isinstance(target.node, Var)
            cpp_t = cpp_type(target.node.type)
            self.emit(f"constexpr {cpp_t} {lhs} = {rhs};")
        else:
            self.emit(f"{lhs} = {rhs};")

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt):
        # a[i] is already a __getitem__/back() CallExpr here, a reference the
        # compound operator can act on directly. Ops with no C++ compound
        # form (/, //, %, **) are rejected earlier, by check_structural.py.
        lhs = self.get_expr(o.lvalue, lvalue=True)
        rhs = self.get_expr(o.rvalue)
        self.emit(f"{lhs} {o.op}= {rhs};")

    def visit_assert_stmt(self, o: AssertStmt):
        condition = self.get_condition(o.expr)
        if o.msg is not None:
            message = self.get_expr(o.msg)
            self.emit(f"if (!({condition})) throw AssertionError(to_str({message}));")
        else:
            self.emit(f'if (!({condition})) throw AssertionError("");')

    def visit_return_stmt(self, o: ReturnStmt):
        if o.expr:
            self.emit(f"return {self.get_expr(o.expr)};")
        else:
            self.emit("return;")

    def visit_if_stmt(self, o: IfStmt):
        # Unlike python ast is a list of if, elif, ..., elif, else
        conditions = o.expr
        bodies = o.body
        for i, (condition, body) in enumerate(zip(conditions, bodies)):
            condition_cpp = self.get_condition(condition)
            if i == 0:
                self.emit(f"if ({condition_cpp}) {{")
            else:
                self.emit(f"}} else if ({condition_cpp}) {{")
            self.visit_block(body)
        if o.else_body:
            self.emit("} else {")
            self.visit_block(o.else_body)
        self.emit("}")

    def visit_for_stmt(self, o: ForStmt):
        translate_for_stmt(self, o)

    def visit_while_stmt(self, o: WhileStmt):
        self.emit(f"while ({self.get_condition(o.expr)}) {{")
        self.visit_block(o.body)
        self.emit("}")

    def visit_try_stmt(self, o: TryStmt):
        translate_try_stmt(self, o)

    def visit_raise_stmt(self, o: RaiseStmt):
        translate_raise_stmt(self, o)

    def visit_expression_stmt(self, o: ExpressionStmt):
        self.emit(f"{self.get_expr(o.expr)};")

    def visit_break_stmt(self, o: BreakStmt):
        self.emit("break;")

    def visit_continue_stmt(self, o: ContinueStmt):
        self.emit("continue;")
