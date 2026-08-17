from pathlib import Path

from mypy.nodes import FuncDef

from splice.analysis.allocation import compute_allocating_functions
from splice.analysis.call_graph import CallEdge, compute_call_graph
from splice.analysis.statement_effects import ExpressionEffect, compute_function_effects
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def test_allocating_functions(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    function_effects = compute_function_effects(program.tree, program.types)
    call_graph = compute_call_graph(program.tree, program.types)
    allocations = compute_allocating_functions(function_effects, call_graph)

    snapshot_lines: list[str] = []
    for funcdef, allocation_cause in allocations.items():
        func = convert_to_python(funcdef)
        cause_text = ""
        for cause in allocation_cause.cause:
            if isinstance(cause, ExpressionEffect):
                cause_text += f"\ndirect: {convert_to_python(cause.node)}"
            else:
                assert isinstance(cause, CallEdge)
                cause_text += f"\ntransitive: {convert_to_python(cause.call)}"
        cause_text += "\n"
        snapshot_lines.append(f"{func}\n{cause_text}")

    full_program = open(test_file).read()
    snap = f"""
{full_program}
--- allocations ---
{"\n".join(snapshot_lines)}
    """
    print(snap)

    functions_by_name = {d.name: d for d in program.tree.defs if isinstance(d, FuncDef)}
    assert set(functions_by_name) == {
        "allocates_directly",
        "calls_allocating",
        "calls_transitively",
        "pure",
        "recursive",
        "main",
    }

    assert functions_by_name["allocates_directly"] in allocations
    assert functions_by_name["calls_allocating"] in allocations
    assert functions_by_name["calls_transitively"] in allocations
    assert functions_by_name["main"] in allocations

    assert functions_by_name["pure"] not in allocations
    assert functions_by_name["recursive"] not in allocations

    assert snap == snapshot
