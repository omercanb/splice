"""Runtime support library real Splice programs import from.

Has to work under plain CPython too, since the same source stays runnable
directly in the interpreter, not just when compiled.
"""

from copy import deepcopy
from typing import TypeVar

_T = TypeVar("_T")


def copy(value: _T) -> _T:
    """A deep copy used for mutable value semantics. Not translated directly to C++"""
    return deepcopy(value)


def _error_after_tree_transform(value: int) -> int:
    """Test-only marker for validate semantics. Used to check that ast positions are correct after tree transforms."""
    return value
