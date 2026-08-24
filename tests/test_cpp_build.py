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
