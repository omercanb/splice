from __future__ import annotations

import os
import re
import subprocess
import sys
import warnings
from dataclasses import dataclass, field
from pathlib import Path
from typing import TYPE_CHECKING, Mapping

RUNTIME_DIR = Path(__file__).parent / "runtime"
PCH_HEADER = RUNTIME_DIR / ".pch.h"
PCH_FILE = RUNTIME_DIR / ".pch.h.pch"
STD = "c++17"


@dataclass
class BuildConfig:
    """Compiler config.
    use_pch is clang specific
    """

    compiler: str = "clang++"
    use_pch: bool = True
    extra_flags: list[str] = field(default_factory=list)


DEFAULT_CONFIG = BuildConfig()


def ensure_pch(config: BuildConfig) -> Path | None:
    """
    Build a precompiled header of runtime/*.h, refreshing it when one changes.
    This is important to have much faster compilation when testing
    """
    if not config.use_pch:
        return None
    # Skip our own generated header, or it ends up including itself.
    headers = sorted(h for h in RUNTIME_DIR.glob("*.h") if h != PCH_HEADER)
    if not headers:
        return None
    if PCH_FILE.exists() and PCH_FILE.stat().st_mtime >= max(
        h.stat().st_mtime for h in headers
    ):
        return PCH_FILE

    PCH_HEADER.write_text("".join(f'#include "{h.name}"\n' for h in headers))
    # Build to a private path and rename, so a parallel worker refreshing at the
    # same time can never leave a half-written pch behind.
    temporary = PCH_FILE.with_suffix(f".{os.getpid()}.tmp")
    built = subprocess.run(
        [
            config.compiler,
            f"-std={STD}",
            f"-I{RUNTIME_DIR}",
            "-fpch-instantiate-templates",
            "-x",
            "c++-header",
            str(PCH_HEADER),
            "-o",
            str(temporary),
        ],
        capture_output=True,
        text=True,
    )
    if built.returncode != 0:
        temporary.unlink(missing_ok=True)
        warnings.warn(
            f"precompiled header failed to build, compiling without it "
            f"(expect a ~4x slower compile):\n{built.stderr}",
            stacklevel=2,
        )
        return None
    os.replace(temporary, PCH_FILE)
    return PCH_FILE


_ERROR_LOCATION = re.compile(r"^(?P<file>.+):(?P<line>\d+):\d+: error:", re.MULTILINE)
_LINE_MARKER = re.compile(r"//\s*(\d+)\s*$")


def _python_lines_for_errors(path: str, stderr: str) -> set[int]:
    """The Python source lines a compile failure's errors trace back to
    each error's C++ line paired with the nearest `// N` marker at or above it"""
    cpp_lines = {
        int(m.group("line"))
        for m in _ERROR_LOCATION.finditer(stderr)
        if m.group("file") == path
    }
    if not cpp_lines:
        return set()

    source_lines = Path(path).read_text().splitlines()
    python_lines: set[int] = set()
    for cpp_line in cpp_lines:
        for i in range(min(cpp_line, len(source_lines)), 0, -1):
            marker = _LINE_MARKER.search(source_lines[i - 1])
            if marker:
                python_lines.add(int(marker.group(1)))
                break
    return python_lines


def _format_python_lines(python_lines: set[int], python_source: str) -> str:
    """Lines corresponding to lines in python source, formatted for user info"""
    source_lines = python_source.splitlines()
    parts = [
        f"IMPORTANT: Error likely corresponds to python line(s): {', '.join(map(str, sorted(python_lines)))}"
    ]
    for line in sorted(python_lines):
        if source_lines is not None and 0 < line <= len(source_lines):
            parts.append(f"  {line}: {source_lines[line - 1]}")
        else:
            parts.append(f"  {line}")
    return "\n".join(parts)


def _is_pch_error(stderr: str) -> bool:
    """Whether a failed compile's stderr points at the pch, not the source."""
    return "PCH file" in stderr or "precompiled header" in stderr


def _rebuild_pch(config: BuildConfig) -> Path | None:
    """Force a fresh precompiled header, discarding whatever is on disk."""
    PCH_FILE.unlink(missing_ok=True)
    return ensure_pch(config)


def compile_cpp(
    path: str,
    exe: str | None = None,
    includes: list[str] | None = None,
    config: BuildConfig | None = None,
    python_source: str | None = None,
) -> subprocess.CompletedProcess:
    """Compile `src` to `exe` using the precompiled header."""
    config = config or DEFAULT_CONFIG
    if exe == None:
        exe = path[: path.rfind(".")]
    directories = [str(RUNTIME_DIR)] + (includes or [])

    def run(pch: Path | None):
        command = (
            [config.compiler, f"-std={STD}"]
            + [f"-I{d}" for d in directories]
            + config.extra_flags
        )
        if pch is not None:
            command += ["-include-pch", str(pch)]
        return subprocess.run(
            command + [path, "-o", exe], capture_output=True, text=True
        )

    compiled = run(ensure_pch(config))
    if compiled.returncode != 0 and _is_pch_error(compiled.stderr):
        compiled = run(_rebuild_pch(config))
    if compiled.returncode != 0 and _is_pch_error(compiled.stderr):
        # Rebuilding didn't help either - drop the pch for this compile
        # rather than fail outright.
        warnings.warn(
            f"precompiled header rejected, recompiling {path} without it "
            f"(expect a slower compile):\n{compiled.stderr}",
            stacklevel=2,
        )
        compiled = run(None)
    if compiled.returncode != 0:
        print(compiled.stderr, end="", file=sys.stderr)
        python_lines = _python_lines_for_errors(path, compiled.stderr)
        if python_lines and python_source:
            print(_format_python_lines(python_lines, python_source), file=sys.stderr)
        raise subprocess.CalledProcessError(
            compiled.returncode, compiled.args, compiled.stdout, compiled.stderr
        )
    return compiled


def compile(
    translated: str,
    output_cpp_path: str,
    output_exe_path: str,
    python_path: str,
    config: BuildConfig | None = None,
) -> str:
    """Write `translated` to `output_cpp_path` and compile it, returning the exe path."""
    python_source = open(python_path).read()
    # output_cpp_path/output_exe_path may live in a directory that doesn't exist yet.
    Path(output_cpp_path).parent.mkdir(parents=True, exist_ok=True)
    Path(output_exe_path).parent.mkdir(parents=True, exist_ok=True)
    Path(output_cpp_path).write_text(translated)

    compiled = compile_cpp(
        output_cpp_path, output_exe_path, config=config, python_source=python_source
    )
    if compiled.stderr:  # warnings still compile
        print("--- compiler warnings ---")
        print(compiled.stderr)

    return output_exe_path


def compile_and_run(
    translated: str,
    output_cpp_path: str,
    output_exe_path: str,
    python_path: str,
    config: BuildConfig | None = None,
) -> subprocess.CompletedProcess:
    """Write `translated` to `output_cpp_path`, compile it, run it, and
    return the captured result."""
    compile(translated, output_cpp_path, output_exe_path, python_path, config=config)

    run_proc = subprocess.run(
        ["stdbuf", "-oL", Path(output_exe_path).resolve()],
        capture_output=True,
        text=True,
    )
    if run_proc.stderr:
        print("--- stderr ---")
        print(run_proc.stderr, end="")
    print(f"--- exit code: {run_proc.returncode} ---")
    return run_proc
