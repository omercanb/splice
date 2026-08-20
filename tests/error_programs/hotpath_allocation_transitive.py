from splice.stdlib import hotpath


def alloc_leaf(a: int, b: int) -> list[int]:
    return [a, b]


def level1_alloc(a: int, b: int) -> list[int]:
    return alloc_leaf(a, b)


@hotpath
def level0_alloc(x: int) -> int:
    pair = level1_alloc(x, x + 1)
    return pair[0]
