def append_call(a: list[int]) -> None:
    a.append(1)


def read_only_call(a: list[int]) -> None:
    a[0]


def constructor_call() -> None:
    a: list[int] = list()


def list_literal() -> None:
    a = [1, 2, 3]


def compound_assign_int(x: int) -> None:
    x += 1


def compound_assign_list(a: list[int], b: list[int]) -> None:
    a += b


def multiple_findings(a: list[int], b: list[int]) -> None:
    a.append(b.pop())


def nested_block(a: list[int], b: list[int]) -> None:
    if a.pop():
        b.append(1)


def transitive_alloc() -> None:
    list_literal()
