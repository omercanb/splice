"""Fixes related to specifics of mypy that are done after the mypy analysis is first done"""
from mypy.nodes import MypyFile, Expression, TempNode, RefExpr, Var, FuncDef, AssignmentStmt, NameExpr, ListExpr, \
    DictExpr, SetExpr
from mypy.types import Type, get_proper_type

from splice.ast_utils import TypeTable
from splice.visitor import Traverser


def get_resolved_types(tree: MypyFile, types: dict[Expression, Type]) -> TypeTable:
    """
    Mypy can resolve types even if they weren't determined when declared, eg: a = [], a.append(1) becomes a list[int]
    However, only the type known at that moment is stored in the type table thats returned from mypy and it instead stores the types in an attribute.
    This function uses those attributes to create the type table that should actually be used for the program
    """
    fixup_partial_literals(tree, types)

    type_table: TypeTable = {}
    temps = 0
    for expr, t in types.items():
        if isinstance(expr, TempNode):
            temps += 1
            # This is an artifact from type checking according to mypy
            continue
        if isinstance(expr, RefExpr) and expr.node is not None:
            # if isinstance(expr.node, TypeInfo):
            #     type_table[expr] = expr.node
            if isinstance(expr.node, Var) or isinstance(expr.node, FuncDef):
                assert expr.node.type is not None
                type_table[expr] = get_proper_type(expr.node.type)
            else:
                type_table[expr] = get_proper_type(types[expr])
        else:
            type_table[expr] = get_proper_type(t)

    assert len(type_table) + temps == len(types), "Some types weren't captured"
    return type_table


def fixup_partial_literals(tree: MypyFile, types: dict[Expression, Type]) -> None:
    """
    Mypy doesn't really try to type container literals
    For example: a = [], a.append(1). Then [] will have type list[Never] while a has type list[int]
    This function provides a type to the container literal
    """

    class _PartialLiteralFinder(Traverser):
        def __init__(self, types: dict[Expression, Type]) -> None:
            self.types = types

        def visit_assignment_stmt(self, o: AssignmentStmt) -> None:
            if len(o.lvalues) == 1:
                lvalue = o.lvalues[0]
                if (
                    isinstance(lvalue, NameExpr)
                    and isinstance(lvalue.node, Var)
                    and isinstance(o.rvalue, (ListExpr, DictExpr, SetExpr))
                ):
                    resolved = lvalue.node.type
                    assert resolved is not None
                    self.types[lvalue] = resolved
                    self.types[o.rvalue] = resolved
            super().visit_assignment_stmt(
                o
            )  # keep recursing into nested functions/blocks

    _PartialLiteralFinder(types).visit(tree)
