def scaled(values: list[int], factor: int) -> list[int]:
    """A comprehension reads the enclosing function's locals and parameters."""
    offset = 1
    return [v * factor + offset for v in values]


def main() -> int:
    numbers = [1, 2, 3, 4]

    # over a name, with and without a condition
    print([v for v in numbers])
    print([v for v in numbers if v > 2])

    # the loop headers the for statement already knew how to build
    print([i for i in range(4)])
    print([i for i in range(1, 8, 2)])
    print([numbers[i] for i in range(len(numbers))])

    # captures
    print(scaled(numbers, 10))

    # set and dict
    squares = {v * v for v in numbers}
    print(sorted(squares))
    lookup = {v: v * v for v in numbers if v > 1}
    print(len(lookup), lookup[2], lookup[4])
    print(lookup)

    # two generators, the second reading the first's index
    print([x * y for x in numbers for y in numbers if x < y])

    # a comprehension inside a comprehension
    print([len([w for w in range(v)]) for v in numbers])

    # a comprehension over one built inline
    doubled = [v * 2 for v in numbers]
    print([v + 1 for v in doubled])

    return 0
