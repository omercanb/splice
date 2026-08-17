from pathlib import Path

from mypy.nodes import CallExpr, FuncDef

from splice.analysis.call_graph import is_call_builtin
from splice.analysis.statement_effects import compute_statment_effects
from splice.convert_to_python import convert_to_python
from splice.pipeline import TypeTable, analyse
from splice.visitor import Traverser

test_file = Path(__file__).parent / "program.py"


class TestBuiltinVisitor(Traverser):
    def __init__(self, types: TypeTable):
        self.calls: list[tuple[CallExpr, bool]] = []
        self.types = types

    def visit_call_expr(self, o: CallExpr) -> None:
        self.calls.append((o, is_call_builtin(o, self.types)))
        return super().visit_call_expr(o)


def test_is_builtin(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    test_visitor = TestBuiltinVisitor(program.types)
    test_visitor.visit(program.tree)
    call_lines = []
    for call, is_builtin in test_visitor.calls:
        python_call = convert_to_python(call)
        builtin_text = "builtin" if is_builtin else "user-defined"
        call_lines.append(str((python_call, builtin_text)))

    assert open(test_file).read() + "\n" + "\n".join(call_lines) == snapshot
