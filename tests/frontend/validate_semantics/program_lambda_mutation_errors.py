def mutates_via_append(matrix: list[list[int]]) -> None:
    list(map(lambda row: row.append(0), matrix))


def mutates_via_pop(items: list[list[int]]) -> None:
    items.sort(key=lambda row: row.pop())
