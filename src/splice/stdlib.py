"""Runtime support library real Splice programs import from.

Has to work under plain CPython too, since the same source stays runnable
directly in the interpreter, not just when compiled.
"""


def _error_after_tree_transform(value: int) -> int:
    """Test-only marker for splice.frontend.validate_semantics.

    Recognised by fullname and flagged with a diagnostic wherever it's
    called, to check that source positions survive the transform pipeline.
    Returns its argument so it can be embedded in an expression (an index,
    a comprehension) rather than only standing alone. Not a real language
    feature - leading underscore.
    """
    return value
