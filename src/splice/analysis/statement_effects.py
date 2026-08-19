"""
Resolves allocations and mutation that happens inside a statement
Used for seeding the call graph based mutation and allocation analysis
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import TypedDict

from mypy.nodes import (
    Block,
    CallExpr,
    ClassDef,
    DictExpr,
    Expression,
    FuncDef,
    ListExpr,
    MemberExpr,
    MypyFile,
    NameExpr,
    OperatorAssignmentStmt,
    SetExpr,
    Statement,
)
from mypy.types import Type

from splice.analysis.builtin_effects import (
    ALLOCATES_ONLY,
    OperationEffect,
    builtin_operation_effect,
    compound_assignment_effect,
    type_name,
)
from splice.codegen.class_def import methods as class_methods
from splice.ast_utils import TypeTable
from splice.visitor import Traverser

_ALLOCATING_CONSTRUCTORS = {"list", "dict", "set"}


@dataclass(frozen=True)
class ExpressionEffect:
    node: Expression
    effect: OperationEffect


class _StatementWalker(Traverser):
    """Finds builtins that allocate and mutate inside a statement"""

    def __init__(self, types: dict[Expression, Type]):
        self.types = types
        self.findings: list[ExpressionEffect] = []

    def visit_call_expr(self, o: CallExpr) -> None:
        if isinstance(o.callee, MemberExpr):
            effect = builtin_operation_effect(
                self.types.get(o.callee.expr), o.callee.name
            )
            if effect is not None:
                self.findings.append(ExpressionEffect(o.callee, effect))
        elif (
            isinstance(o.callee, NameExpr) and o.callee.name in _ALLOCATING_CONSTRUCTORS
        ):
            self.findings.append(ExpressionEffect(o, ALLOCATES_ONLY))
        super().visit_call_expr(o)

    def visit_operator_assignment_stmt(self, o: OperatorAssignmentStmt) -> None:
        lvalue_type = type_name(self.types.get(o.lvalue))
        self.findings.append(
            ExpressionEffect(o.lvalue, compound_assignment_effect(lvalue_type or ""))
        )
        super().visit_operator_assignment_stmt(o)

    def visit_list_expr(self, o: ListExpr) -> None:
        self.findings.append(ExpressionEffect(o, ALLOCATES_ONLY))
        super().visit_list_expr(o)

    def visit_dict_expr(self, o: DictExpr) -> None:
        self.findings.append(ExpressionEffect(o, ALLOCATES_ONLY))
        super().visit_dict_expr(o)

    def visit_set_expr(self, o: SetExpr) -> None:
        self.findings.append(ExpressionEffect(o, ALLOCATES_ONLY))
        super().visit_set_expr(o)


def compute_statment_effects(
    stmt: Statement, types: dict[Expression, Type]
) -> list[ExpressionEffect]:
    walker = _StatementWalker(types)
    walker.visit(stmt)
    return walker.findings


def compute_function_effects(
    tree: MypyFile, types: TypeTable
) -> dict[FuncDef, list[ExpressionEffect]]:
    funcdefs: list[FuncDef] = []
    for definition in tree.defs:
        if isinstance(definition, FuncDef):
            funcdefs.append(definition)
        elif isinstance(definition, ClassDef):
            funcdefs.extend(class_methods(definition))

    effects = {}
    for funcdef in funcdefs:
        visitor = _StatementWalker(types)
        visitor.visit(funcdef)
        effects[funcdef] = visitor.findings
    return effects
