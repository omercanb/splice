from pathlib import Path

from mypy.nodes import ClassDef, FuncDef, MypyFile

from splice.analysis.call_graph import compute_call_graph
from splice.analysis.mutation import (
    MutationTable,
    TransitiveMutation,
    compute_mutating_parameters,
)
from splice.analysis.statement_effects import ExpressionEffect, compute_function_effects
from splice.codegen.class_def import methods as class_methods
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def _collect_functions(tree: MypyFile) -> dict[str, FuncDef]:
    """Methods are keyed as 'Box.fill', plain functions as 'hi'."""
    functions: dict[str, FuncDef] = {}
    for definition in tree.defs:
        if isinstance(definition, FuncDef):
            functions[definition.name] = definition
        elif isinstance(definition, ClassDef):
            for method in class_methods(definition):
                functions[f"{definition.name}.{method.name}"] = method
    return functions


def _mutation_lines(name: str, funcdef: FuncDef, mutations: MutationTable) -> list[str]:
    lines: list[str] = []
    for arg in funcdef.arguments:
        fact = mutations.get(arg.variable)
        if fact is None:
            continue
        lines.append(f"{name}({arg.variable.name}):")
        for cause in fact.cause:
            if isinstance(cause, ExpressionEffect):
                lines.append(f"  direct: {convert_to_python(cause.node)}")
            else:
                assert isinstance(cause, TransitiveMutation)
                lines.append(f"  transitive: {convert_to_python(cause.edge.call)}")
    return lines


def test_mutating_parameters(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    function_effects = compute_function_effects(program.tree, program.types)
    call_graph = compute_call_graph(program.tree, program.types)
    mutations = compute_mutating_parameters(function_effects, call_graph)

    functions = _collect_functions(program.tree)

    def is_mutable(func_name: str, param_name: str) -> bool:
        funcdef = functions[func_name]
        var = next(
            a.variable for a in funcdef.arguments if a.variable.name == param_name
        )
        return var in mutations

    assert is_mutable("mutates_directly", "items")

    assert is_mutable("calls_mutating", "items")
    assert is_mutable("calls_transitively", "items")

    assert not is_mutable("pure", "a")
    assert not is_mutable("pure", "b")

    assert not is_mutable("recursive_pure", "n")

    assert is_mutable("recursive_mutating", "items")
    assert not is_mutable("recursive_mutating", "n")

    assert is_mutable("mutates_via_compound_assign", "items")

    assert is_mutable("mutates_via_index", "matrix")

    assert is_mutable("mutates_via_tuple_index", "pair")

    assert is_mutable("mutates_via_back", "items")

    assert is_mutable("mutates_element_indirectly", "matrix")
    assert is_mutable("mutate_row", "row")

    assert is_mutable("mutates_via_index_assign", "items")

    assert is_mutable("mutates_via_index_compound_assign", "items")

    assert is_mutable("mixed_mutability", "mutated")
    assert not is_mutable("mixed_mutability", "untouched")

    assert is_mutable("calls_mixed_mutability", "a")
    assert not is_mutable("calls_mixed_mutability", "b")

    assert is_mutable("Box.fill", "self")
    assert is_mutable("Box.relay", "self")
    assert not is_mutable("Box.fill", "x")
    assert not is_mutable("Box.relay", "x")

    assert is_mutable("Box.__init__", "self")

    assert is_mutable("mutates_field_directly", "c")
    assert is_mutable("calls_field_mutator", "c")

    assert is_mutable("mutates_array_field", "b")

    lines: list[str] = []
    for definition in program.tree.defs:
        if isinstance(definition, FuncDef):
            lines.append(convert_to_python(definition))
            lines.extend(_mutation_lines(definition.name, definition, mutations))
        elif isinstance(definition, ClassDef):
            lines.append(convert_to_python(definition))
            for method in class_methods(definition):
                name = f"{definition.name}.{method.name}"
                lines.extend(_mutation_lines(name, method, mutations))
        lines.append("")

    snap = "\n".join(lines)

    assert snap == snapshot
