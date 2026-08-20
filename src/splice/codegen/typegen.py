from typing import Optional

from mypy.types import (
    AnyType,
    Instance,
    LiteralType,
    NoneType,
    TupleType,
    Type,
    UnionType,
    get_proper_type,
)

from splice.ast_utils import literal_int_value
from splice.codegen.builtins import FIXED_WIDTH_INT_TYPES


class UnsupportedType(Exception):
    """A mypy type with no C++ equivalent.

    Raised rather than asserted so validation can catch it and report the node
    it came from, keeping the list of convertible types in one place.
    """

    def __init__(self, t: Type, problematic_member_type: Optional[Type] = None):
        self.type = t
        self.problematic_member_type = problematic_member_type
        super().__init__(f"no C++ equivalent for the type {t}")


def cpp_type(t: Type) -> str:
    return cpp_type_name(t)


def cpp_type_name(t: Type) -> str:
    """Convert a mypy type to C++ type string.

    Uses pattern matching to handle different type kinds.
    """
    t = get_proper_type(t)
    current_error: Optional[Exception] = None

    try:
        match t:
            # Builtin types
            case Instance(type=type_info) if type_info.fullname == "builtins.int":
                return "int64_t"
            case Instance(type=type_info) if type_info.fullname == "builtins.float":
                return "double"
            case Instance(type=type_info) if type_info.fullname == "builtins.str":
                return "str"
            case Instance(type=type_info) if type_info.fullname == "builtins.bytes":
                return "bytes"
            # Some cases an int is typed as a typing.SupportsIndex
            case Instance(type=type_info) if (
                type_info.fullname == "typing.SupportsIndex"
            ):
                return "int64_t"
            case Instance(type=type_info) if type_info.fullname == "builtins.bool":
                return "bool"
            case Instance(type=type_info) if type_info.fullname in FIXED_WIDTH_INT_TYPES:
                return FIXED_WIDTH_INT_TYPES[type_info.fullname]

            # Container types
            case Instance(type=type_info, args=args) if (
                type_info.fullname == "builtins.list" and args
            ):
                elem_type = cpp_type(args[0])
                return f"list<{elem_type}>"

            case Instance(type=type_info, args=args) if (
                type_info.fullname == "builtins.dict" and len(args) >= 2
            ):
                key_type = cpp_type(args[0])
                val_type = cpp_type(args[1])
                return f"dict<{key_type}, {val_type}>"

            case Instance(type=type_info, args=args) if (
                type_info.fullname == "builtins.set" and args
            ):
                elem_type = cpp_type(args[0])
                return f"set<{elem_type}>"

            case Instance(type=type_info, args=args) if (
                type_info.fullname == "splice.stdlib.Array" and len(args) == 2
            ):
                elem_type = cpp_type(args[0])
                n = literal_int_value(args[1])
                if n is None:
                    current_error = UnsupportedType(t)
                    raise current_error
                return f"Array<{elem_type}, {n}>"

            # Tuple with fixed elements
            case TupleType(items=items):
                elem_types = ", ".join(cpp_type(item) for item in items)
                return f"tuple<{elem_types}>"

            # Optional[T] = T | None
            case UnionType(items=items) if len(items) == 2 and any(
                isinstance(item, NoneType) for item in items
            ):
                non_none = next(
                    item for item in items if not isinstance(item, NoneType)
                )
                inner = cpp_type(non_none)
                return f"std::optional<{inner}>"

            # A literal is just its underlying type: `x = 3` infers Literal[3].
            case LiteralType(fallback=fallback):
                return cpp_type_name(fallback)

            # Generic union
            case UnionType(items=items):
                types = [cpp_type_name(item) for item in items]
                # Members can differ in mypy yet agree in C++, as Literal[0] and
                # Literal[3] both do. Only a real disagreement needs a variant.
                if len(set(types)) == 1:
                    return types[0]
                return f"std::variant<{', '.join(types)}>"

            # None/void
            case NoneType():
                return "void"

            # Any type
            case AnyType():
                return "auto"

            # open() hands back a TextIOWrapper, which the runtime calls `file`.
            case Instance(type=type_info) if type_info.fullname == "_io.TextIOWrapper":
                return "file"

            case Instance(type=type_info) if type_info.fullname == "builtins.object":
                current_error = UnsupportedType(t)
                raise current_error

            case Instance(type=type_info):
                return type_info.name

            # Iterator/Iterable
            case Instance(type=type_info, args=args) if (
                "Iterator" in type_info.fullname and args
            ):
                elem_type = cpp_type_name(args[0])
                return f"Iterator<{elem_type}>"

            # Default fallback
            case _:
                current_error = UnsupportedType(t)
                raise current_error
    except UnsupportedType as e:
        # Propagate the error up while keeping the member type that actually
        # caused it: leave it alone where it was raised, set it one frame up,
        # then just re-raise from there.
        if e.problematic_member_type:
            raise UnsupportedType(t, problematic_member_type=e.problematic_member_type)
        elif e is current_error:
            raise
        else:
            raise UnsupportedType(t, problematic_member_type=e.type)
