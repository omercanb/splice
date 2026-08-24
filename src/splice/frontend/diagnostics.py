"""Compiler warnings and errors"""

from __future__ import annotations

from dataclasses import dataclass
from enum import StrEnum

from mypy.nodes import Context


class Severity(StrEnum):
    ERROR = "error"
    WARNING = "warning"


@dataclass(frozen=True)
class Diagnostic:
    """One unsupported construct and the source span it occupies."""

    kind: str
    message: str
    hint: str
    line: int
    column: int
    end_line: int
    end_column: int
    # WARNING reports but never blocks compilation, unlike the default.
    severity: Severity = Severity.ERROR

    @property
    def position(self) -> tuple[int, int]:
        return self.line, self.column


def diagnostic(
    node: Context,
    kind: str,
    message: str,
    hint: str,
    severity: Severity = Severity.ERROR,
) -> Diagnostic:
    """Build a diagnostic from any mypy node, which carries its own span."""
    end_line = node.end_line if node.end_line is not None else node.line
    end_column = node.end_column if node.end_column is not None else node.column + 1
    return Diagnostic(
        kind=kind,
        message=message,
        hint=hint,
        line=node.line,
        column=node.column,
        end_line=end_line,
        end_column=end_column,
        severity=severity,
    )


class UnsupportedProgram(Exception):
    """The set of compiler errors in a program"""

    def __init__(self, diagnostics: list[Diagnostic]):
        self.diagnostics = diagnostics
        super().__init__(f"{len(diagnostics)} unsupported construct(s)")


def render(diagnostics: list[Diagnostic], source: str, path: str = "<source>") -> str:
    """Render diagnostics with the offending source line underlined."""
    lines = source.splitlines()
    return "\n".join(_render_one(d, lines, path) for d in diagnostics)


def _render_one(diagnostic: Diagnostic, lines: list[str], path: str) -> str:
    text = lines[diagnostic.line - 1] if diagnostic.line <= len(lines) else ""
    # A span running onto later lines is underlined to the end of the first.
    end = diagnostic.end_column if diagnostic.end_line == diagnostic.line else len(text)
    underline = " " * diagnostic.column + "^" * max(1, end - diagnostic.column)

    number = str(diagnostic.line)
    gutter = " " * len(number)
    # mypy columns are 0 based, editors and compilers count from 1.
    header = (
        f"{path}:{diagnostic.line}:{diagnostic.column + 1}: "
        f"{diagnostic.severity}: {diagnostic.message}"
    )
    hint_lines = diagnostic.hint.splitlines()
    hint = "\n".join(
        [f"  help: {hint_lines[0]}"] + [f"          {l}" for l in hint_lines[1:]]
    )
    return (
        f"{header}\n"
        f"\n"
        f"  {number} | {text}\n"
        f"  {gutter} | {underline}\n"
        f"\n"
        f"{hint}\n"
    )
