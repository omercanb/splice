"""
Resolves allocations and mutation that happens inside a statement
Used for seeding the call graph based mutation and allocation analysis
"""

from __future__ import annotations

from dataclasses import dataclass

from mypy.nodes import (
    Block,
    CallExpr,
    DictExpr,
    Expression,
    ListExpr,
    MemberExpr,
    NameExpr,
    OperatorAssignmentStmt,
    SetExpr,
    Statement,
)
from mypy.types import Instance, Type, get_proper_type

from splice.analysis.builtin_effects import (
    ALLOCATES_ONLY,
    OperationEffect,
    builtin_operation_effect,
    compound_assignment_effect,
)
from splice.visitor import Traverser

_ALLOCATING_CONSTRUCTORS = {"list", "dict", "set"}


@dataclass(frozen=True)
class Finding:
    node: Expression
    effect: OperationEffect


def _type_name(t: Type | None) -> str | None:
    """Returns the name of the class if t is an object"""
    if t is None:
        return None
    proper = get_proper_type(t)
    return proper.type.name if isinstance(proper, Instance) else None


class _StatementWalker(Traverser):
    """Finds builtins that allocate and mutate inside a statement"""

    def __init__(self, types: dict[Expression, Type]):
        self.types = types
        self.findings: list[Finding] = []

    def visit_block(self, o: Block) -> None:
        pass  # nested blocks are the outer walker's job, not this one's

    def visit_call_expr(self, o: CallExpr) -> None:
        if isinstance(o.callee, MemberExpr):
            receiver_type = _type_name(self.types.get(o.callee.expr))
            effect = (
                builtin_operation_effect(receiver_type, o.callee.name)
                if receiver_type is not None
                else None
            )
            if effect is not None:
                self.findings.append(Finding(o.callee.expr, effect))
        elif (
            isinstance(o.callee, NameExpr) and o.callee.name in _ALLOCATING_CONSTRUCTORS
        ):
            self.findings.append(Finding(o, ALLOCATES_ONLY))
        super().visit_call_expr(o)

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> None:
        lvalue_type = _type_name(self.types.get(o.lvalue))
        self.findings.append(
            Finding(o.lvalue, compound_assignment_effect(lvalue_type or ""))
        )
        super().visit_operator_assignment_stmt(o)

    def visit_list_expr(self, o: ListExpr) -> None:
        self.findings.append(Finding(o, ALLOCATES_ONLY))
        super().visit_list_expr(o)

    def visit_dict_expr(self, o: DictExpr) -> None:
        self.findings.append(Finding(o, ALLOCATES_ONLY))
        super().visit_dict_expr(o)

    def visit_set_expr(self, o: SetExpr) -> None:
        self.findings.append(Finding(o, ALLOCATES_ONLY))
        super().visit_set_expr(o)


def analyze_statement(stmt: Statement, types: dict[Expression, Type]) -> list[Finding]:
    walker = _StatementWalker(types)
    walker.visit(stmt)
    return walker.findings
