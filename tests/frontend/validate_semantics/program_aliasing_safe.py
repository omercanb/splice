def mutate(a: list[int], b: list[int]) -> None:
    a.append(1)


def read_only(a: list[int], b: list[int]) -> None:
    print(a[0], b[0])


def alias_read_only_args(items: list[int]) -> None:
    read_only(items, items)


def no_alias_different_roots(a: list[int], b: list[int]) -> None:
    mutate(a, b)


def no_alias_distinct_literal_indices(matrix: list[list[int]]) -> None:
    mutate(matrix[0], matrix[1])


def builtin_mutate_no_alias(a: list[int], b: list[int]) -> None:
    a.extend(b)


def builtin_read_only_alias_ok(items: list[int]) -> None:
    print(items.index(items[0]))


def no_alias_via_copy(items: list[int]) -> None:
    mutate(items.copy(), items)


def compound_assign_no_alias(a: list[int], b: list[int]) -> None:
    a += b
