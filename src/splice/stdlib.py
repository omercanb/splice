"""Runtime support library real Splice programs import from.

Has to work under plain CPython too, since the same source stays runnable
directly in the interpreter, not just when compiled.
"""

from copy import deepcopy
from typing import Generic, TypeVar, get_args

T = TypeVar("T")
N = TypeVar("N", bound=int)


def copy(value: T) -> T:
    """A deep copy used for mutable value semantics. Not translated directly to C++"""
    return deepcopy(value)


def hotpath(func: T) -> T:
    """Marks a function/method as hot codegen emits it with FLATTEN. Does nothing when run as Python"""
    return func


class Array(Generic[T, N]):
    """A fixed size array"""

    # The __class_getitem__ hack is how we get Array[int, 10]() producing an instance
    # Array[int, 10] returns an array alias, which then gets called with no arguments

    def __class_getitem__(cls, params):
        elem_type, size = params
        if get_args(size):
            # If N is a literal[N] extract the number
            # Otherwise its a final int
            (size,) = get_args(size)
        return _ArrayAlias(elem_type, size)

    def __init__(self, elem_type, size):
        self.data = [elem_type() for _ in range(size)]

    def __getitem__(self, index):
        return self.data[index]

    def __setitem__(self, index, item):
        self.data[index] = item

    def __len__(self):
        return len(self.data)

    def __iter__(self):
        return iter(self.data)

    def __str__(self):
        return str(self.data)

    def fill(self, item):
        for i in range(len(self.data)):
            self.data[i] = copy(item)


class _ArrayAlias:
    def __init__(self, elem_type, size):
        self.elem_type = elem_type
        self.size = size

    def __call__(self):
        return Array(self.elem_type, self.size)


# Fixed-width integers, shaped like mypy_extensions' i64/i32/i16/u8.
# __new__ returns a plain int, so these stay ordinary ints at runtime, with
# the same isinstance behavior as int. Operators are declared in stdlib.pyi.
class _FixedWidthIntMeta(type):
    def __instancecheck__(cls, inst):
        return isinstance(inst, int)


_sentinel = object()


class int8(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class uint8(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class int16(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class uint16(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class int32(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class uint32(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class int64(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


class uint64(metaclass=_FixedWidthIntMeta):
    def __new__(cls, x=0, base=_sentinel):
        if base is not _sentinel:
            return int(x, base)
        return int(x)


def _error_after_tree_transform(value: int) -> int:
    """Test-only marker for the mutable-value-semantics check. Used to check that ast positions are correct after tree transforms."""
    return value
