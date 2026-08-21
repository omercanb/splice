def return_param(a: list[int]) -> list[int]:
    return a


def assign_local_from_local() -> int:
    a: list[int] = [1, 2, 3]
    b = a
    return b[0]


class Box:
    value: list[int]

    def __init__(self) -> None:
        self.value = []


def return_field_of_param(b: Box) -> list[int]:
    return b.value
