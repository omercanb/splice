def mutate_directly(values: list[int]) -> None:
    values[0] = 99


def level1(values: list[int]) -> None:
    mutate_directly(values)


def level0(values: list[int], other: int) -> None:
    level1(values)


def use_it(matrix: list[list[int]]) -> None:
    level0(matrix[0], matrix[0][0])
