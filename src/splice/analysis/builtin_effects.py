"""Known (mutates, allocates) facts for builtin operations.

The seed for the mutation/allocation analysis - the base case for
operations whose implementation isn't Splice source the analysis can walk.
Cross-checked against src/splice/runtime/{list,dict,set,str,bytes}.h.

Three sources feed the seed, each a different AST shape, so the caller
(analyze_statement, built elsewhere) picks the right lookup for whatever
kind of node it's looking at - none of this is one uniform table:

- Method calls: BUILTIN_OPERATION_EFFECTS, keyed by (container type,
  method name), type-directed the same way codegen already resolves
  methods (translation_utils.call_method), not by mypy fullname.
- OperatorAssignmentStmt (`+=`, `*=`): compound_assignment_effect(). No
  receiver-shaped call here, so not keyed by method name.
- Constructors and literals (`[1, 2, 3]`, `list(...)`): always
  ALLOCATES_ONLY, recognized by node type (ListExpr/DictExpr/SetExpr, or a
  call `should_wrap_call_in_pointer` already flags in
  expression_codegen.py) - no receiver, nothing to look up here at all.
  Tuple literals are the one exception: `tuple` wraps std::tuple directly,
  not ptr<T>-wrapped, so they don't allocate.
"""

from dataclasses import dataclass


@dataclass(frozen=True)
class OperationEffect:
    mutates: bool
    allocates: bool


READS = OperationEffect(mutates=False, allocates=False)
MUTATES_ONLY = OperationEffect(mutates=True, allocates=False)
MUTATES_AND_ALLOCATES = OperationEffect(mutates=True, allocates=True)
ALLOCATES_ONLY = OperationEffect(mutates=False, allocates=True)  # builds a new value; receiver/scope untouched

# str and bytes have no mutating methods at all - every one is const and
# returns a new value. Checked ahead of the per-method table rather than
# listing all ~40 of them individually.
STRUCTURALLY_IMMUTABLE_TYPES = {"str", "bytes"}

BUILTIN_OPERATION_EFFECTS: dict[tuple[str, str], OperationEffect] = {
    # list
    ("list", "__getitem__"): READS,
    ("list", "back"): READS,
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
    ("set", "union_"): ALLOCATES_ONLY,  # `union` is a C++ keyword; codegen rewrites s.union(...) to this
    ("set", "intersection"): ALLOCATES_ONLY,
    ("set", "difference"): ALLOCATES_ONLY,
    ("set", "symmetric_difference"): ALLOCATES_ONLY,
}


def builtin_operation_effect(container_type: str, method_name: str) -> OperationEffect | None:
    if container_type in STRUCTURALLY_IMMUTABLE_TYPES:
        return READS
    return BUILTIN_OPERATION_EFFECTS.get((container_type, method_name))


# Types whose += / *= can grow the underlying buffer. frontend.validate
# already rejects any compound operator with no direct C++ form (OP_MAP),
# so anything reaching an OperatorAssignmentStmt compiles to a real
# compound assignment and unconditionally mutates its lvalue - only
# whether it can also allocate depends on the type.
COMPOUND_ASSIGN_ALLOCATING_TYPES = {"list", "str", "bytes"}


def compound_assignment_effect(lvalue_type: str) -> OperationEffect:
    return OperationEffect(
        mutates=True, allocates=lvalue_type in COMPOUND_ASSIGN_ALLOCATING_TYPES
    )
