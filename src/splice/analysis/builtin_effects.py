"""
Functions for querying wether builtin operations mutate or allocate

Includes handling for builtin methods, literal expressions like [1, 2, 3] and constructors like list()
"""

from dataclasses import dataclass


@dataclass(frozen=True)
class OperationEffect:
    mutates: bool
    allocates: bool


READS = OperationEffect(mutates=False, allocates=False)
MUTATES_ONLY = OperationEffect(mutates=True, allocates=False)
MUTATES_AND_ALLOCATES = OperationEffect(mutates=True, allocates=True)
ALLOCATES_ONLY = OperationEffect(mutates=False, allocates=True)

# str and bytes have no mutating methods at all
STRUCTURALLY_IMMUTABLE_TYPES = {"str", "bytes"}

BUILTIN_OPERATION_EFFECTS: dict[tuple[str, str], OperationEffect] = {
    # list
    ("list", "__getitem__"): READS,
    ("list", "back"): READS,  # A function we define for efficient negative index access
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
}


def builtin_operation_effect(
    container_type: str, method_name: str
) -> OperationEffect | None:
    if container_type in STRUCTURALLY_IMMUTABLE_TYPES:
        return READS
    return BUILTIN_OPERATION_EFFECTS.get((container_type, method_name))


COMPOUND_ASSIGN_ALLOCATING_TYPES = {"list", "str", "bytes"}


def compound_assignment_effect(lvalue_type: str) -> OperationEffect:
    return OperationEffect(
        mutates=True, allocates=lvalue_type in COMPOUND_ASSIGN_ALLOCATING_TYPES
    )
