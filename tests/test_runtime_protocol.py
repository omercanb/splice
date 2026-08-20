"""Every runtime class a program can hold has to render itself.

print() renders its arguments with py::to_str, which is a free function only
for the primitives; everything else is a class of ours and supplies __str__.
A class without one only breaks the day someone first prints one, so the
headers are checked here instead.
"""

import glob
import re
from pathlib import Path

runtime_path = "src/splice/runtime"

# Classes no program can hold, so nothing can ever print one. Each says why:
# adding a class here is a deliberate call, not a way to silence the test.
NOT_PRINTABLE = {
    "Finally": "a scope guard, only ever a local that runs its body",
    "FormatArg": "for formatting",
    "FormatArgs": "for formatting",
    "FormatSpec": "for formatting",
    "hasher": "a hash policy handed to std::unordered_map",
    "has_hash_method": "a type trait",
    "has_bool_conversion": "a type trait",
    "has_bool_method": "a type trait",
    "has_len_method": "a type trait",
    "has_str": "a type trait",
    "has_repr": "a type trait",
    "comparable": "a type trait",
    "list_storage": "a type trait, picks list<T>'s backing container",
    "bool_vector": "list<bool>'s backing storage, never held on its own",
    "slice": "only ever built inline for x[a:b], never bound to a name",
    "dict_iterator": "iteration state, never reachable from Python",
    "file_iterator": "iteration state, never reachable from Python",
    "list_iterator": "iteration state, never reachable from Python",
    "array_iterator": "iteration state, never reachable from Python",
    "set_iterator": "iteration state, never reachable from Python",
    "str_iterator": "iteration state, never reachable from Python",
    "bytes_iterator": "iteration state, never reachable from Python",
    "tuple_iterator": "iteration state, never reachable from Python",
    "range_iterator": "iteration state, never reachable from Python",
    "enumerate_iter": "iteration state, never reachable from Python",
    "zip_iter": "iteration state, never reachable from Python",
    "map_iter": "iteration state, never reachable from Python",
    "filter_iter": "iteration state, never reachable from Python",
    "reversed_iter": "iteration state, never reachable from Python",
}

# repr() falls back to __str__ for everything, so a class only needs its own
# __repr__ when it has to render differently inside a container.
ALSO_NEEDS_REPR = {"str": "elements show quoted, so {'a': 1} rather than {a: 1}"}

CLASS = re.compile(
    r"^[ \t]*(?:template\s*<[^>]*>\s*)?(?:class|struct)\s+(\w+)\s*(?::([^{;]*))?\{",
    re.M,
)
_BASE_NOISE = {"public", "private", "protected", "virtual", "std"}


def headers() -> str:
    return "\n".join(Path(p).read_text() for p in sorted(glob.glob(f"{runtime_path}/*.h")))


def _body(source: str, start: int) -> str:
    """The text of a class body, from its opening brace to the matching one."""
    depth = 0
    for end in range(start, len(source)):
        if source[end] == "{":
            depth += 1
        elif source[end] == "}":
            depth -= 1
            if depth == 0:
                return source[start : end + 1]
    return source[start:]


def declared_classes(source: str) -> dict[str, dict]:
    """Every class in the runtime, with the bases and body text it declares."""
    found: dict[str, dict] = {}
    for match in CLASS.finditer(source):
        name, bases = match.group(1), match.group(2)
        found[name] = {
            "bases": [
                b for b in re.findall(r"\w+", bases or "") if b not in _BASE_NOISE
            ],
            "body": _body(source, match.end() - 1),
        }
    return found


def defines(method: str, name: str, classes: dict, source: str) -> bool:
    """Whether a class declares a method, inline in its body or out of line."""
    declared = re.search(rf"\b{method}\s*\(", classes[name]["body"]) is not None
    out_of_line = re.search(rf"\b{name}::{method}\s*\(", source) is not None
    if declared or out_of_line:
        return True
    return any(
        base in classes and defines(method, base, classes, source)
        for base in classes[name]["bases"]
    )


def test_every_class_renders_itself():
    source = headers()
    classes = declared_classes(source)
    missing = [
        name
        for name in classes
        if name not in NOT_PRINTABLE and not defines("__str__", name, classes, source)
    ]
    assert not missing, (
        f"no __str__ on {', '.join(sorted(missing))}. Give the class one so "
        f"py::to_str picks it up, or list it in NOT_PRINTABLE with the reason "
        f"a program can never hold one."
    )


def test_classes_needing_repr_have_one():
    source = headers()
    classes = declared_classes(source)
    missing = [
        name
        for name in ALSO_NEEDS_REPR
        if not defines("__repr__", name, classes, source)
    ]
    assert not missing, f"no __repr__ on {', '.join(sorted(missing))}"


def test_exemptions_are_live():
    """An exemption naming a class that no longer exists is stale."""
    stale = set(NOT_PRINTABLE) - set(declared_classes(headers()))
    assert not stale, f"NOT_PRINTABLE names classes that are gone: {sorted(stale)}"
