def mutates_directly(items: list[int]) -> None:
    items.append(1)


def calls_mutating(items: list[int]) -> None:
    mutates_directly(items)


def calls_transitively(items: list[int]) -> None:
    calls_mutating(items)


def pure(a: int, b: int) -> int:
    return a + b


def recursive_pure(n: int) -> int:
    if n <= 1:
        return 1
    return n * recursive_pure(n - 1)


def recursive_mutating(items: list[int], n: int) -> None:
    if n <= 0:
        return
    items.append(n)
    recursive_mutating(items, n - 1)


def mutates_via_compound_assign(items: list[int]) -> None:
    items += [1]


def mutates_via_index(matrix: list[list[int]]) -> None:
    matrix[0].append(1)


def mutates_via_tuple_index(pair: tuple[list[int], int]) -> None:
    pair[0].append(1)


def mutates_via_back(items: list[list[int]]) -> None:
    items[-1].append(1)


def mutate_row(row: list[int]) -> None:
    row.append(1)


def mutates_element_indirectly(matrix: list[list[int]]) -> None:
    mutate_row(matrix[0])


def mutates_via_index_assign(items: list[int]) -> None:
    items[0] = 5


def mutates_via_index_compound_assign(items: list[int]) -> None:
    items[0] *= 2


def mixed_mutability(mutated: list[int], untouched: list[int]) -> None:
    mutated.append(1)


def calls_mixed_mutability(a: list[int], b: list[int]) -> None:
    mixed_mutability(a, b)


class Box:
    items: list[int]

    def __init__(self) -> None:
        self.items = []

    def fill(self, x: int) -> None:
        self.items.append(x)

    def relay(self, x: int) -> None:
        self.fill(x)
