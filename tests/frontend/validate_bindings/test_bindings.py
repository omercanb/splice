"""The stateful checks in validate_bindings.py: parameter reassignment and
reading a for loop's variable after the loop that bound it has closed.
"""

from pathlib import Path

import pytest

from splice.frontend.diagnostics import UnsupportedProgram, render
from splice.pipeline import analyse, pipeline

parameter_reassignment_path = (
    Path(__file__).parent / "program_parameter_reassignment_errors.py"
)
stale_loop_variable_path = Path(__file__).parent / "program_stale_loop_variable_errors.py"
stale_loop_variable_safe_path = (
    Path(__file__).parent / "program_stale_loop_variable_safe.py"
)


def test_parameter_reassignment_errors(snapshot):
    """Every case in program_parameter_reassignment_errors.py is rejected."""
    source = parameter_reassignment_path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(str(parameter_reassignment_path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 3
    assert all(d.kind == "parameter-reassignment" for d in diagnostics)
    assert (
        render(diagnostics, source, parameter_reassignment_path.name) == snapshot
    )


def test_stale_loop_variable_errors(snapshot):
    """Every case in program_stale_loop_variable_errors.py is rejected."""
    source = stale_loop_variable_path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(str(stale_loop_variable_path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 1
    assert all(d.kind == "stale-loop-variable" for d in diagnostics)
    assert render(diagnostics, source, stale_loop_variable_path.name) == snapshot


def test_stale_loop_variable_safe():
    """Two sequential loops reusing the same variable name don't false-positive -
    each loop's own body reads its own live variable, not a stale one left over
    from the other loop.
    """
    source = stale_loop_variable_safe_path.read_text()
    analyse(str(stale_loop_variable_safe_path), source)
