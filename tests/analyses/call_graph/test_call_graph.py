from pathlib import Path

from splice.analysis.call_graph import CallGraphProducer
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def test_call_graph(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    producer = CallGraphProducer(program.types)
    producer.visit(program.tree)

    edge_lines = []
    for edge in producer.call_graph.edges:
        binding_text = ", ".join(
            f"{convert_to_python(expr)} -> {var.name}" for expr, var in edge.bindings
        )
        edge_lines.append(
            f"{edge.caller.name} -> {edge.callee.name}: "
            f"{convert_to_python(edge.call)} [{binding_text}]"
        )

    assert open(test_file).read() + "\n" + "\n".join(edge_lines) == snapshot
