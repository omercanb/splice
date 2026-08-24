"""check_structural.py's check_supported_call: rejecting calls to real Python
functions the runtime doesn't implement, while leaving the user's own
functions - and functions declared in their own .pyi extern module - alone.
"""

from pathlib import Path

import pytest

from splice.frontend.static_checks.compiler_errors_warnings import UnsupportedProgram
from splice.pipeline import analyse

extern_call_safe_path = Path(__file__).parent / "program_extern_call_safe.py"


def test_extern_call_safe(monkeypatch):
    """A call to a function declared in the user's own .pyi extern module is
    never treated as an unsupported builtin - it's trusted like any other
    real, filesystem-resolvable module.

    mypy resolves `import extern_mod` by searching the current directory, the
    same way it would in a real project where the .pyi sits next to main.py -
    so the test runs from this directory rather than the repo root.
    """
    monkeypatch.chdir(extern_call_safe_path.parent)
    source = extern_call_safe_path.read_text()
    result = analyse(extern_call_safe_path.name, source)
    assert result.diagnostics == []


def test_unsupported_stdlib_module_errors():
    """A call into a real stdlib module we know of but haven't implemented
    (math) is rejected, unlike a call into the user's own extern module.
    """
    path = Path(__file__).parents[2] / "error_programs" / "unsupported_stdlib_module.py"
    source = path.read_text()
    with pytest.raises(UnsupportedProgram) as raised:
        analyse(str(path), source)
    diagnostics = raised.value.diagnostics
    assert len(diagnostics) == 1
    assert diagnostics[0].kind == "unsupported-builtin"
