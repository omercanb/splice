from __future__ import annotations

import os
import subprocess
import sys
import tempfile
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
            f"(expect a ~4x slower compile):\n{compiled.stderr}",
            stacklevel=2,
        )
        compiled = run(None)
    if compiled.returncode != 0:
        print(compiled.stderr, end="", file=sys.stderr)
        raise subprocess.CalledProcessError(
            compiled.returncode, compiled.args, compiled.stdout, compiled.stderr
        )
    return compiled


def compile_proc(
    translated: str,
    src="main.cpp",
    exe=None,
    config: BuildConfig | None = None,
) -> str:
    if exe == None:
        exe = src[: src.rfind(".")]
    # src/exe may live in a directory that doesn't exist yet.
    Path(src).parent.mkdir(parents=True, exist_ok=True)
    Path(exe).parent.mkdir(parents=True, exist_ok=True)
    Path(src).write_text(translated)

    compiled = compile_cpp(src, exe, config=config)
    if compiled.returncode != 0:
        print("--- compile FAILED ---")
        print(compiled.stderr)
        raise ValueError("Compilation failed")
    if compiled.stderr:  # warnings still compile
        print("--- compiler warnings ---")
        print(compiled.stderr)

    return exe


def build_and_run(
    translated: str, src="main.cpp", exe="main", config: BuildConfig | None = None
):
    """Write `translated` to a .cpp file, compile with g++, run it, print output."""
    compile_proc(translated, src, exe, config=config)
    run_proc = subprocess.run(["stdbuf", "-oL", str(Path(exe).resolve())])
    # print("--- program output ---")
    # print(run_proc.stdout, end="")
    if run_proc.stderr:
        print("--- stderr ---")
        print(run_proc.stderr, end="")
    print(f"--- exit code: {run_proc.returncode} ---")


def build_and_run_capture(
    translated: str,
    src: str | None = None,
    exe: str | None = None,
    config: BuildConfig | None = None,
) -> subprocess.CompletedProcess:
    """Write `translated` to a .cpp file, compile with g++, run it, print output.

    Defaults to a fresh directory per call, so parallel test workers don't
    overwrite each other's main.cpp/main. Pass src/exe to write somewhere
    specific.
    """
    with tempfile.TemporaryDirectory() as directory:
        src = src or f"{directory}/main.cpp"
        exe = exe or f"{directory}/main"
        compile_proc(translated, src, exe, config=config)

        run_proc = subprocess.run(
            ["stdbuf", "-oL", Path(exe).resolve()], capture_output=True, text=True
        )
    # print("--- program output ---")
    # print(run_proc.stdout, end="")
    if run_proc.stderr:
        print("--- stderr ---")
        print(run_proc.stderr, end="")
    print(f"--- exit code: {run_proc.returncode} ---")
    return run_proc
