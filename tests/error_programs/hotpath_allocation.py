from splice.stdlib import hotpath


@hotpath
def make_list(x: int) -> list[int]:
    return [x, x + 1]
