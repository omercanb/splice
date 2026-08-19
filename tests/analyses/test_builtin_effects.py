from mypy.nodes import FuncDef
from mypy.types import Type

from splice.analysis.builtin_effects import (
    builtin_operation_effect,
    compound_assignment_effect,
)
from splice.pipeline import analyse


def _param_type(source: str) -> Type:
    """The type of `x`, the first parameter of the first function in source."""
    program = analyse("t.py", source)
    funcdef = next(d for d in program.tree.defs if isinstance(d, FuncDef))
    param_type = funcdef.arguments[0].variable.type
    assert param_type is not None
    return param_type


def test_read_only_operation_has_no_effect():
    t = _param_type("def f(x: list[int]) -> None: pass\n")
    effect = builtin_operation_effect(t, "__getitem__")
    assert effect is not None
    assert not effect.mutates
    assert not effect.allocates


def test_in_place_write_mutates_without_allocating():
    t = _param_type("def f(x: list[int]) -> None: pass\n")
    effect = builtin_operation_effect(t, "__setitem__")
    assert effect is not None
    assert effect.mutates
    assert not effect.allocates


def test_growing_operation_mutates_and_allocates():
    t = _param_type("def f(x: list[int]) -> None: pass\n")
    effect = builtin_operation_effect(t, "append")
    assert effect is not None
    assert effect.mutates
    assert effect.allocates


def test_new_value_from_existing_allocates_without_mutating():
    t = _param_type("def f(x: dict[str, int]) -> None: pass\n")
    effect = builtin_operation_effect(t, "keys")
    assert effect is not None
    assert not effect.mutates
    assert effect.allocates


def test_str_and_bytes_have_no_mutating_method():
    for annotation in ("str", "bytes"):
        t = _param_type(f"def f(x: {annotation}) -> None: pass\n")
        for method_name in ("upper", "strip", "split", "anything_at_all"):
            effect = builtin_operation_effect(t, method_name)
            assert effect is not None
            assert not effect.mutates


def test_unknown_operation_returns_none():
    list_type = _param_type("def f(x: list[int]) -> None: pass\n")
    assert builtin_operation_effect(list_type, "sort_by_magic") is None

    widget_type = _param_type(
        "class Widget:\n"
        "    def __init__(self) -> None:\n"
        "        pass\n"
        "\n"
        "def f(x: Widget) -> None: pass\n"
    )
    assert builtin_operation_effect(widget_type, "append") is None


def test_compound_assign_always_mutates():
    for lvalue_type in ("list", "str", "bytes", "int", "float", "Widget"):
        assert compound_assignment_effect(lvalue_type).mutates


def test_compound_assign_allocates_only_for_growable_types():
    assert compound_assignment_effect("list").allocates
    assert compound_assignment_effect("str").allocates
    assert compound_assignment_effect("bytes").allocates
    assert not compound_assignment_effect("int").allocates
    assert not compound_assignment_effect("float").allocates
