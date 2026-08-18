class Item:
    value: int

    def __init__(self) -> None:
        self.value = 0


class Container:
    items: list[Item]

    def __init__(self) -> None:
        self.items = []


class Pair:
    x: int
    y: int

    def __init__(self) -> None:
        self.x = 0
        self.y = 0


class Wrapper:
    pair: Pair

    def __init__(self) -> None:
        self.pair = Pair()


def plain_name(c: Container) -> Container:
    return c


def field_access(c: Container) -> list[Item]:
    return c.items


def index_access(c: Container) -> Item:
    return c.items[0]


def nested_field(c: Container) -> int:
    return c.items[0].value


def last_index_field(c: Container) -> int:
    return c.items[-1].value


def not_a_place(c: Container) -> int:
    return len(c.items)


def alias_same_root(c: Container) -> tuple[Container, Container]:
    return (c, c)


def alias_field_vs_root(c: Container) -> tuple[list[Item], Container]:
    return (c.items, c)


def alias_list_vs_index(c: Container) -> tuple[list[Item], Item]:
    return (c.items, c.items[0])


def alias_identical_nested(c: Container) -> tuple[int, int]:
    return (c.items[0].value, c.items[0].value)


def no_alias_diverge_nested(w: Wrapper) -> tuple[int, int]:
    return (w.pair.x, w.pair.y)


def no_alias_different_fields(p: Pair) -> tuple[int, int]:
    return (p.x, p.y)


def no_alias_different_roots(a: Container, b: Container) -> tuple[list[Item], list[Item]]:
    return (a.items, b.items)


def alias_dynamic_indices(items: list[int], i: int, j: int) -> tuple[int, int]:
    return (items[i], items[j])


def alias_same_literal_index(items: list[int]) -> tuple[int, int]:
    return (items[0], items[0])


def no_alias_literal_indices(items: list[int]) -> tuple[int, int]:
    return (items[0], items[1])


def alias_negative_index_vs_itself(items: list[int]) -> tuple[int, int]:
    return (items[-1], items[-1])


def alias_negative_index_vs_positive(items: list[int]) -> tuple[int, int]:
    return (items[-1], items[0])
