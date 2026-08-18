"""Analysis on a call graphs that checks which function parameters get mutated"""

from collections import defaultdict
from dataclasses import dataclass, field

from mypy.nodes import (
    CallExpr,
    Expression,
    FuncDef,
    IndexExpr,
    MemberExpr,
    NameExpr,
    Var,
)

from splice.analysis.call_graph import CallEdge, CallGraph
from splice.analysis.statement_effects import ExpressionEffect


@dataclass(frozen=True)
class TransitiveMutation:
    edge: CallEdge
    callee_param: Var  # The mutating param in the called function


@dataclass
class MutatesFact:
    cause: list[ExpressionEffect | TransitiveMutation] = field(default_factory=list)


# Vars are parameters
type MutationTable = dict[Var, MutatesFact]

_ALIASING_METHODS = {"__getitem__"}


def get_structural_base(expr: Expression) -> Var | None:
    """Get the base of an access expression, eg a.field.__getitem__(0)"""
    if isinstance(expr, NameExpr):
        node = expr.node
        return node if isinstance(node, Var) else None
    if isinstance(expr, MemberExpr):
        return get_structural_base(expr.expr)
    if isinstance(expr, IndexExpr):
        return get_structural_base(expr.base)
    if isinstance(expr, CallExpr) and isinstance(expr.callee, MemberExpr):
        if expr.callee.name in _ALIASING_METHODS:
            return get_structural_base(expr.callee.expr)
    return None


def compute_mutating_parameters(
    function_effects: dict[FuncDef, list[ExpressionEffect]],
    call_graph: CallGraph,
) -> MutationTable:

    # Seed the inital mutating parameters
    mutated_params: MutationTable = defaultdict(MutatesFact)
    for funcdef, funcdef_effects in function_effects.items():
        for effect in funcdef_effects:
            if not effect.effect.mutates:
                continue
            base = get_structural_base(effect.node)
            if base is None:
                continue
            if base in {arg.variable for arg in funcdef.arguments}:
                mutated_params[base].cause.append(effect)

    # Where a parameter is passed in from calling functions
    parameter_sources: dict[Var, list[tuple[CallEdge, Expression]]] = defaultdict(list)
    for edge in call_graph.edges:
        for caller_expr, callee_var in edge.bindings:
            parameter_sources[callee_var].append((edge, caller_expr))

    # If a function mutates it parameter, callers of that function need to be checked
    # For the calling edges, check the argument corresponding to the mutating parameter
    # If its structural base is a parameter, add it to the working set and continue
    # Propogate allocations to callers

    working_set = list(mutated_params)
    while working_set:
        mutated_param = working_set.pop()
        for call_edge, argument in parameter_sources[mutated_param]:
            passed_argument_base = get_structural_base(argument)
            if passed_argument_base is None:
                continue
            # Is the argument thats passed in a parameter of the function
            if passed_argument_base in {
                arg.variable for arg in call_edge.caller.arguments
            }:
                if passed_argument_base not in mutated_params:
                    working_set.append(passed_argument_base)

                mutated_params[passed_argument_base].cause.append(
                    TransitiveMutation(call_edge, mutated_param)
                )

    return mutated_params
