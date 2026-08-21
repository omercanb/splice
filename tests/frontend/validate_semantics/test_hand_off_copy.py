"""The mandatory-copy check in validate_semantics.py: check_hand_off_copy,
covering the parameter-vs-local distinction on return and the scalar
exemption.
"""

from pathlib import Path

import pytest

from splice.frontend.validate import UnsupportedProgram, render
from splice.pipeline import analyse, pipeline

errors_path = Path(__file__).parent / "program_hand_off_errors.py"
safe_path = Path(__file__).parent / "program_hand_off_safe.py"


def test_hand_off_errors(snapshot):
    """Every case in program_hand_off_errors.py is rejected."""
    source = errors_path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(str(errors_path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 3
    assert all(d.kind == "missing-copy" for d in diagnostics)
    assert render(diagnostics, source, errors_path.name) == snapshot


def test_hand_off_safe():
    """None of these trigger the mandatory-copy check - no false positives."""
    source = safe_path.read_text()
    analyse(str(safe_path), source)
