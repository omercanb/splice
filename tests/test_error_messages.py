"""Every unsupported construct reports where it is and what to write instead.

The rendered message is snapshotted, so any change to the wording, the span or
the hint shows up as a reviewable diff. Run with --snapshot-update to accept.
"""

import ast
import glob
from pathlib import Path

import pytest

from splice.frontend.static_checks.compiler_errors_warnings import (
    UnsupportedProgram,
    render,
)
from splice.pipeline import analyse

error_programs_path = "tests/error_programs"
paths = sorted(glob.glob(f"{error_programs_path}/*.py"))

check_paths = [
    "src/splice/frontend/static_checks/check_structural.py",
    "src/splice/frontend/static_checks/check_mutable_value_semantics.py",
    "src/splice/frontend/static_checks/check_bindings.py",
]


def _diagnostics_for(path: str):
    """A program's diagnostics, whether they were fatal or just warnings."""
    source = Path(path).read_text()
    try:
        result = analyse(path, source)
    except UnsupportedProgram as raised:
        return source, raised.diagnostics
    return source, result.diagnostics


@pytest.mark.parametrize("path", paths, ids=lambda p: Path(p).name)
def test_error_message(path: str, snapshot):
    """A rejected program renders exactly the message recorded for it."""
    source, diagnostics = _diagnostics_for(path)
    assert render(diagnostics, source, Path(path).name) == snapshot


def reportable_kinds() -> set[str]:
    """Every kind the checks can report, read off their self.report calls."""
    return {
        node.args[1].value
        for check_path in check_paths
        for node in ast.walk(ast.parse(Path(check_path).read_text()))
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
