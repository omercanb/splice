def mutate(a: list[int], b: int) -> None:
    a.append(1)


def use(matrix: list[list[int]]) -> None:
    mutate(matrix[0], matrix[0][0])
