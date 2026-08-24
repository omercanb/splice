"""The exclusivity checks in check_mutable_value_semantics.py: check_exclusivity and
check_builtin_exclusivity for regular vs. builtin calls, plus the for-loop
container check and the compound-assignment check.
"""

from pathlib import Path

import pytest

from splice.frontend.static_checks.compiler_errors_warnings import (
    UnsupportedProgram,
    render,
)
from splice.pipeline import analyse, pipeline

errors_path = Path(__file__).parent / "program_aliasing_errors.py"
safe_path = Path(__file__).parent / "program_aliasing_safe.py"


def test_aliasing_errors(snapshot):
    """Every case in program_aliasing_errors.py is rejected."""
    source = errors_path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(str(errors_path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 9
    assert all(
        d.kind in ("aliasing-arguments", "aliasing-loop-container") for d in diagnostics
    )
    assert render(diagnostics, source, errors_path.name) == snapshot


def test_aliasing_safe():
    """None of these trigger the exclusivity check - no false positives."""
    source = safe_path.read_text()
    analyse(str(safe_path), source)
