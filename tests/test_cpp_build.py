"""cpp_build.py: mapping a real C++ compile failure back to the Python
source line it traces to, via the `// N` markers StatementCodegen emits.
"""

import subprocess
from pathlib import Path

import pytest

from splice.cpp_build import _format_python_lines, _python_lines_for_errors, compile_cpp
from splice.pipeline import pipeline

test_dir = Path(__file__).parent / "cpp_build"
program_path = test_dir / "program_broken_extern.py"
print_program_path = test_dir / "program_broken_print.py"


def test_broken_extern_call_maps_to_python_line(tmp_path, monkeypatch, snapshot):
    """extern_mod.pyi declares broken_function but no C++ implementation
    exists anywhere - the compile fails, and the failure should trace back
    to the two Python lines that called it.
    """
    monkeypatch.chdir(test_dir)
    source = program_path.read_text()
    cpp = pipeline(program_path.name, source)

    cpp_path = tmp_path / "main.cpp"
    cpp_path.write_text(cpp)

    with pytest.raises(subprocess.CalledProcessError) as raised:
        compile_cpp(str(cpp_path))

    python_lines = _python_lines_for_errors(str(cpp_path), raised.value.stderr)
    message = _format_python_lines(python_lines, source)
    assert message == snapshot


def test_broken_print_maps_to_python_line_via_instantiation_note(
    tmp_path, monkeypatch, snapshot
):
    """print()'s to_str() call fails inside a runtime template (print.h), so
    the primary `error:` is never on main.cpp itself - only a `note: in
    instantiation of ... requested here` a few lines later is. The mapping
    has to pick that up too, not just error: lines.
    """
    monkeypatch.chdir(test_dir)
    source = print_program_path.read_text()
    cpp = pipeline(print_program_path.name, source)

    cpp_path = tmp_path / "main.cpp"
    cpp_path.write_text(cpp)

    with pytest.raises(subprocess.CalledProcessError) as raised:
        compile_cpp(str(cpp_path))

    python_lines = _python_lines_for_errors(str(cpp_path), raised.value.stderr)
    message = _format_python_lines(python_lines, source)
    assert message == snapshot
