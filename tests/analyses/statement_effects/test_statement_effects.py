from pathlib import Path

from mypy.nodes import FuncDef

from splice.analysis.statement_effects import compute_statment_effects
from splice.convert_to_python import convert_to_python
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def test_statement_effects_snapshot(snapshot):
    program = analyse(str(test_file), open(test_file).read())
    lines = []
    for d in program.tree.defs:
        if not isinstance(d, FuncDef):
            continue
        stmt = d.body.body[0]
        findings = compute_statment_effects(stmt, program.types)
        lines.append(f"{d.name}:")
        if not findings:
            lines.append("  (no findings)")
        for f in findings:
            lines.append(
                f"  {convert_to_python(f.node)} -> "
                f"mutates={f.effect.mutates} allocates={f.effect.allocates}"
            )
    assert open(test_file).read() + "\n" + "\n".join(lines) == snapshot
