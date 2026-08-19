def mutate(a: list[int], b: list[int]) -> None:
    a.append(1)


def alias_mutable_args(items: list[int]) -> None:
    mutate(items, items)


def alias_nested_args(matrix: list[list[int]]) -> None:
    mutate(matrix[0], matrix[0])


def builtin_extend_self_alias(items: list[int]) -> None:
    items.extend(items)


def builtin_append_nested_alias(items: list[int]) -> None:
    items.append(items[0])


def compound_assign_self_alias(items: list[int]) -> None:
    items += items


def compound_assign_nested_alias(matrix: list[list[int]]) -> None:
    matrix += matrix[0:1]
