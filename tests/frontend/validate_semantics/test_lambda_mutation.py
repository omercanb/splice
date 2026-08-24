"""The lambda-mutates-its-own-parameter check in check_mutable_value_semantics.py:
check_lambda_no_mutation. A lambda passed to a builtin like sorted/map/filter is
trusted (a flat seed fact in builtin_effects.py) to never mutate what it's given -
this is what makes that trust sound.
"""

from pathlib import Path

import pytest

from splice.frontend.static_checks.compiler_errors_warnings import (
    UnsupportedProgram,
    render,
)
from splice.pipeline import analyse, pipeline

errors_path = Path(__file__).parent / "program_lambda_mutation_errors.py"
safe_path = Path(__file__).parent / "program_lambda_mutation_safe.py"


def test_lambda_mutation_errors(snapshot):
    """Every case in program_lambda_mutation_errors.py is rejected."""
    source = errors_path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        pipeline(str(errors_path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 2
    assert all(d.kind == "lambda-mutates-parameter" for d in diagnostics)
    assert render(diagnostics, source, errors_path.name) == snapshot


def test_lambda_mutation_safe():
    """None of these trigger the check - no false positives."""
    source = safe_path.read_text()
    analyse(str(safe_path), source)
