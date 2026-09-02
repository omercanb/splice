"""
Functions for querying wether builtin operations mutate or allocate

Includes handling for builtin methods, literal expressions like [1, 2, 3] and constructors like list()
"""

from dataclasses import dataclass

from mypy.types import Instance, Type, get_proper_type


@dataclass(frozen=True)
class OperationEffect:
    mutates: bool
    allocates: bool
    # Which positional argument indices this operation mutates, self counted
    # as index 0 for a method call. Every current entry only ever mutates
    # self, but this is tracked by index rather than assumed, so a future
    # operation that mutates a different argument is handled the same way.
    mutated_args: frozenset[int] = frozenset()


READS = OperationEffect(mutates=False, allocates=False)
MUTATES_ONLY = OperationEffect(mutates=True, allocates=False, mutated_args=frozenset({0}))
MUTATES_AND_ALLOCATES = OperationEffect(
    mutates=True, allocates=True, mutated_args=frozenset({0})
)
ALLOCATES_ONLY = OperationEffect(mutates=False, allocates=True)

# str and bytes have no mutating methods at all
STRUCTURALLY_IMMUTABLE_TYPES = {"str", "bytes"}

BUILTIN_OPERATION_EFFECTS: dict[tuple[str, str], OperationEffect] = {
    # list
    ("list", "__getitem__"): READS,
    ("list", "__getslice__"): READS,
    ("list", "index"): READS,
    ("list", "count"): READS,
    ("list", "__contains__"): READS,
    ("list", "__setitem__"): MUTATES_ONLY,
    ("list", "__delitem__"): MUTATES_ONLY,
    ("list", "pop"): MUTATES_ONLY,
    ("list", "remove"): MUTATES_ONLY,
    ("list", "clear"): MUTATES_ONLY,
    ("list", "sort"): MUTATES_ONLY,
    ("list", "reverse"): MUTATES_ONLY,
    ("list", "append"): MUTATES_AND_ALLOCATES,
    ("list", "extend"): MUTATES_AND_ALLOCATES,
    ("list", "insert"): MUTATES_AND_ALLOCATES,
    # dict
    ("dict", "__getitem__"): READS,
    ("dict", "get"): READS,
    ("dict", "__contains__"): READS,
    ("dict", "__delitem__"): MUTATES_ONLY,
    ("dict", "pop"): MUTATES_ONLY,
    ("dict", "popitem"): MUTATES_ONLY,
    ("dict", "clear"): MUTATES_ONLY,
    ("dict", "__setitem__"): MUTATES_AND_ALLOCATES,
    ("dict", "setdefault"): MUTATES_AND_ALLOCATES,
    ("dict", "update"): MUTATES_AND_ALLOCATES,
    ("dict", "keys"): ALLOCATES_ONLY,
    ("dict", "values"): ALLOCATES_ONLY,
    ("dict", "items"): ALLOCATES_ONLY,
    # set
    ("set", "__contains__"): READS,
    ("set", "issubset"): READS,
    ("set", "issuperset"): READS,
    ("set", "isdisjoint"): READS,
    ("set", "remove"): MUTATES_ONLY,
    ("set", "discard"): MUTATES_ONLY,
    ("set", "pop"): MUTATES_ONLY,
    ("set", "clear"): MUTATES_ONLY,
    ("set", "intersection_update"): MUTATES_ONLY,
    ("set", "difference_update"): MUTATES_ONLY,
    ("set", "add"): MUTATES_AND_ALLOCATES,
    ("set", "update"): MUTATES_AND_ALLOCATES,
    ("set", "symmetric_difference_update"): MUTATES_AND_ALLOCATES,
    (
        "set",
        "union_",
    ): ALLOCATES_ONLY,  # `union` is a C++ keyword; codegen rewrites s.union(...) to this
    ("set", "intersection"): ALLOCATES_ONLY,
    ("set", "difference"): ALLOCATES_ONLY,
    ("set", "symmetric_difference"): ALLOCATES_ONLY,
    # Array - fixed-size, so never allocates (__getslice__ returns a new
    # list<T>, but list's own __getslice__ is classified READS too - matched
    # for consistency, not because it's actually allocation-free).
    ("Array", "__getitem__"): READS,
    ("Array", "__len__"): READS,
    ("Array", "__contains__"): READS,
    ("Array", "index"): READS,
    ("Array", "count"): READS,
    ("Array", "__getslice__"): READS,
    ("Array", "__setitem__"): MUTATES_ONLY,
    ("Array", "fill"): MUTATES_ONLY,
    ("Array", "sort"): MUTATES_ONLY,
    ("Array", "reverse"): MUTATES_ONLY,
}


def type_name(t: Type | None) -> str | None:
    """The class name if t is an object, otherwise None."""
    if t is None:
        return None
    proper = get_proper_type(t)
    return proper.type.name if isinstance(proper, Instance) else None


def builtin_operation_effect(t: Type | None, method_name: str) -> OperationEffect | None:
    container_type = type_name(t)
    if container_type is None:
        return None
    if container_type in STRUCTURALLY_IMMUTABLE_TYPES:
        return READS
    return BUILTIN_OPERATION_EFFECTS.get((container_type, method_name))


COMPOUND_ASSIGN_ALLOCATING_TYPES = {"list", "str", "bytes"}


def compound_assignment_effect(lvalue_type: str) -> OperationEffect:
    return OperationEffect(
        mutates=True, allocates=lvalue_type in COMPOUND_ASSIGN_ALLOCATING_TYPES
    )
