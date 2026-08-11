from splice.analysis.builtin_effects import (
    builtin_operation_effect,
    compound_assignment_effect,
)


def test_read_only_operation_has_no_effect():
    effect = builtin_operation_effect("list", "__getitem__")
    assert effect is not None
    assert not effect.mutates
    assert not effect.allocates


def test_in_place_write_mutates_without_allocating():
    effect = builtin_operation_effect("list", "__setitem__")
    assert effect is not None
    assert effect.mutates
    assert not effect.allocates


def test_growing_operation_mutates_and_allocates():
    effect = builtin_operation_effect("list", "append")
    assert effect is not None
    assert effect.mutates
    assert effect.allocates


def test_new_value_from_existing_allocates_without_mutating():
    effect = builtin_operation_effect("dict", "keys")
    assert effect is not None
    assert not effect.mutates
    assert effect.allocates


def test_str_and_bytes_have_no_mutating_method():
    for container_type in ("str", "bytes"):
        for method_name in ("upper", "strip", "split", "anything_at_all"):
            effect = builtin_operation_effect(container_type, method_name)
            assert effect is not None
            assert not effect.mutates


def test_unknown_operation_returns_none():
    assert builtin_operation_effect("list", "sort_by_magic") is None
    assert builtin_operation_effect("Widget", "append") is None


def test_compound_assign_always_mutates():
    for lvalue_type in ("list", "str", "bytes", "int", "float", "Widget"):
        assert compound_assignment_effect(lvalue_type).mutates


def test_compound_assign_allocates_only_for_growable_types():
    assert compound_assignment_effect("list").allocates
    assert compound_assignment_effect("str").allocates
    assert compound_assignment_effect("bytes").allocates
    assert not compound_assignment_effect("int").allocates
    assert not compound_assignment_effect("float").allocates
