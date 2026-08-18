from pathlib import Path

import pytest
from mypy.nodes import (
    ArgKind,
    CallExpr,
    Expression,
    FuncDef,
    IntExpr,
    MemberExpr,
    NameExpr,
    ReturnStmt,
    TupleExpr,
    Var,
)

from splice.analysis.structural_aliasing import (
    AccessPath,
    get_access_path,
    is_access_path_structural_alias,
)
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"

# Every function below returns a 2-tuple of the expressions to compare.
ALIAS_CASES = [
    "alias_same_root",
    "alias_field_vs_root",
    "alias_list_vs_index",
    "alias_identical_nested",
    "alias_dynamic_indices",
    "alias_same_literal_index",
    "alias_back_vs_back",
    "alias_back_vs_getitem",
]
NO_ALIAS_CASES = [
    "no_alias_diverge_nested",
    "no_alias_different_fields",
    "no_alias_different_roots",
    "no_alias_literal_indices",
]


def _pair_expr(funcdef: FuncDef) -> tuple[Expression, Expression]:
    """Every case function is a single `return (expr1, expr2)`."""
    (stmt,) = funcdef.body.body
    assert isinstance(stmt, ReturnStmt)
    assert isinstance(stmt.expr, TupleExpr)
    e1, e2 = stmt.expr.items
    return e1, e2


def test_structural_alias(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    functions = {d.name: d for d in program.tree.defs if isinstance(d, FuncDef)}

    def paths(name: str) -> tuple[AccessPath, AccessPath]:
        e1, e2 = _pair_expr(functions[name])
        p1, p2 = get_access_path(e1), get_access_path(e2)
        assert p1 is not None and p2 is not None
        return p1, p2

    def alias(name: str) -> AccessPath | None:
        p1, p2 = paths(name)
        return is_access_path_structural_alias(p1, p2)

    for name in ALIAS_CASES:
        assert alias(name) is not None, f"{name} should alias"
    for name in NO_ALIAS_CASES:
        assert alias(name) is None, f"{name} should not alias"

    # Order shouldn't change the verdict, for both a conflict and a clean case.
    p1, p2 = paths("alias_field_vs_root")
    assert (is_access_path_structural_alias(p1, p2) is None) == (
        is_access_path_structural_alias(p2, p1) is None
    )
    p1, p2 = paths("no_alias_different_fields")
    assert (is_access_path_structural_alias(p1, p2) is None) == (
        is_access_path_structural_alias(p2, p1) is None
    )

    lines: list[str] = []
    for name in ALIAS_CASES + NO_ALIAS_CASES:
        path1, path2 = paths(name)
        path1_text = convert_to_python(
            path1.projections[-1] if path1.projections else path1.root
        )
        path2_text = convert_to_python(
            path2.projections[-1] if path2.projections else path2.root
        )
        lines.append(f"{path1_text}, {path2_text}")
        result = alias(name)
        if result is None:
            lines.append(f"{name}: no alias")
            continue
        parts = [f"root: {result.root.name}"]
        if result.projections:
            parts.append(convert_to_python(result.projections[-1]))
        lines.append(f"{name}: {', '.join(parts)}")

    snap = "\n".join(lines)

    assert snap == snapshot


def test_different_projection_kinds_never_alias():
    """A named field and a getitem/back call can never denote the same
    storage, even at the same chain position with the same root - unlike
    back() vs __getitem__, there's no type-level reason they could collide.
    """
    root = Var("x")
    field_path = AccessPath(root, [MemberExpr(NameExpr("x"), "field")])
    call_path = AccessPath(
        root,
        [
            CallExpr(
                MemberExpr(NameExpr("x"), "__getitem__"),
                [IntExpr(0)],
                [ArgKind.ARG_POS],
                [None],
            )
        ],
    )
    assert is_access_path_structural_alias(field_path, call_path) is None


def test_unrecognized_aliasing_method_combination_asserts():
    """_ALIASING_METHODS is only ever {"back", "__getitem__"} today, so this
    can't happen via get_access_path - this proves it fails loudly rather
    than silently doing nothing if a third aliasing method is ever added.
    """
    root = Var("x")
    path1 = AccessPath(
        root, [CallExpr(MemberExpr(NameExpr("x"), "pop_front"), [], [], [])]
    )
    path2 = AccessPath(root, [CallExpr(MemberExpr(NameExpr("x"), "back"), [], [], [])])
    with pytest.raises(AssertionError):
        is_access_path_structural_alias(path1, path2)
