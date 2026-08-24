def reads_via_len(matrix: list[list[int]]) -> None:
    list(map(lambda row: len(row), matrix))


def reads_via_index(matrix: list[list[int]]) -> None:
    matrix.sort(key=lambda row: row[0])


def mutates_something_else(matrix: list[list[int]], other: list[int]) -> None:
    list(map(lambda row: other.append(row[0]), matrix))
