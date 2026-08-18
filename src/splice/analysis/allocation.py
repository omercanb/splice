"""Analysis on a call graph that check which functions allocate transitively"""

from collections import defaultdict
from dataclasses import dataclass, field

from mypy.nodes import FuncDef

from splice.analysis.call_graph import CallEdge, CallGraph
from splice.analysis.statement_effects import ExpressionEffect


@dataclass
class AllocatesFact:
    """Represent that a function allocates and which expression allocates"""

    # An allocation is either an effect of a builtin, or transitive
    cause: list[ExpressionEffect | CallEdge] = field(default_factory=list)


def compute_allocating_functions(
    function_effects: dict[FuncDef, list[ExpressionEffect]],
    call_graph: CallGraph,
) -> dict[FuncDef, AllocatesFact]:

    # Seed the inital allocating functions
    allocating_functions: dict[FuncDef, AllocatesFact] = defaultdict(AllocatesFact)
    for funcdef, funcdef_effects in function_effects.items():
        for effect in funcdef_effects:
            if effect.effect.allocates:
                allocating_functions[funcdef].cause.append(effect)

    # Function calls which to the funcdef thats the key
    callers: dict[FuncDef, list[CallEdge]] = defaultdict(list)
    for edge in call_graph.edges:
        callers[edge.callee].append(edge)

    # Propogate allocations to callers
    working_set = list(allocating_functions)
    while working_set:
        cur_function = working_set.pop()
        for call_edge in callers[cur_function]:
            if call_edge.caller not in allocating_functions:
                working_set.append(call_edge.caller)
            allocating_functions[call_edge.caller].cause.append(call_edge)

    return allocating_functions
