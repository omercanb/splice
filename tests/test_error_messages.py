"""Every unsupported construct reports where it is and what to write instead.

The rendered message is snapshotted, so any change to the wording, the span or
the hint shows up as a reviewable diff. Run with --snapshot-update to accept.
"""

import ast
import glob
from pathlib import Path

import pytest

from splice.pipeline import pipeline
from splice.frontend.validate import UnsupportedProgram, render

error_programs_path = "tests/error_programs"
paths = sorted(glob.glob(f"{error_programs_path}/*.py"))

validator_paths = [
    "src/splice/frontend/validate.py",
    "src/splice/frontend/validate_semantics.py",
]


def _diagnostics_for(path: str):
    source = Path(path).read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(path, source)
    return source, raised.value.diagnostics


@pytest.mark.parametrize("path", paths, ids=lambda p: Path(p).name)
def test_error_message(path: str, snapshot):
    """A rejected program renders exactly the message recorded for it."""
    source, diagnostics = _diagnostics_for(path)
    assert render(diagnostics, source, Path(path).name) == snapshot


def reportable_kinds() -> set[str]:
    """Every kind the validators can report, read off their self.report calls."""
    return {
        node.args[1].value
        for validator_path in validator_paths
        for node in ast.walk(ast.parse(Path(validator_path).read_text()))
        if isinstance(node, ast.Call)
        and isinstance(node.func, ast.Attribute)
        and node.func.attr == "report"
        and len(node.args) > 1
        and isinstance(node.args[1], ast.Constant)
    }


def test_every_diagnostic_has_a_program():
    """A new check without a program here would ship with no message tested."""
    covered: set[str] = set()
    for path in paths:
        _, diagnostics = _diagnostics_for(path)
        covered.update(diagnostic.kind for diagnostic in diagnostics)
    assert reportable_kinds() - covered == set()
