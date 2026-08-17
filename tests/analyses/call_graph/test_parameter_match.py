from pathlib import Path

from mypy.nodes import CallExpr, Expression, Var

from splice.analysis.call_graph import is_call_builtin, match_call_arguments
from splice.convert_to_python import convert_to_python
from splice.pipeline import TypeTable, analyse
from splice.visitor import Traverser

test_file = Path(__file__).parent / "program.py"


class TestCallGraphVisitor(Traverser):
    def __init__(self, types: TypeTable):
        self.types = types
        self.calls: list[tuple[CallExpr, list[tuple[Expression, Var]]]] = []

    def visit_call_expr(self, o: CallExpr) -> None:
        if not is_call_builtin(o, self.types):
            self.calls.append((o, match_call_arguments(o, self.types)))
        return super().visit_call_expr(o)


def test_parameter_match(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    test_visitor = TestCallGraphVisitor(program.types)
    test_visitor.visit(program.tree)
    call_lines = []
    for call, bindings in test_visitor.calls:
        python_call = convert_to_python(call)
        binding_text = ", ".join(
            f"{convert_to_python(expr)} -> {var.name}" for expr, var in bindings
        )
        call_lines.append(f"({python_call!r}, [{binding_text}])")

    assert open(test_file).read() + "\n" + "\n".join(call_lines) == snapshot
