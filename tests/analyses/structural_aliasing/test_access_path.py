from pathlib import Path

from mypy.nodes import Expression, FuncDef, MypyFile, ReturnStmt

from splice.analysis.structural_aliasing import get_access_path
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def _return_expr(funcdef: FuncDef) -> Expression:
    """Every function in program.py is a single `return <expr>`."""
    (stmt,) = funcdef.body.body
    assert isinstance(stmt, ReturnStmt)
    assert stmt.expr is not None
    return stmt.expr


def _format_access_path(name: str, expr: Expression) -> str:
    lines = [f"{name}:"]
    path = get_access_path(expr)
    if path is None:
        lines.append("  not an access path")
        return "\n".join(lines)
    lines.append(f"  root: {path.root.name}")
    for projection in path.projections:
        lines.append(f"  {convert_to_python(projection)}")
    return "\n".join(lines)


def test_access_path(snapshot):
    program = analyse(str(test_file), open(test_file).read(), check_semantics=False)

    lines: list[str] = []
    for definition in program.tree.defs:
        if not isinstance(definition, FuncDef):
            continue
        expr = _return_expr(definition)
        lines.append(convert_to_python(definition))
        lines.append(_format_access_path(definition.name, expr))
        lines.append("")

    snap = "\n".join(lines)

    assert snap == snapshot
