def main() -> int:
    matrix: list[list[int]] = [[1], [2], [3]]
    list(map(lambda row: row.append(0), matrix))
    return 0
