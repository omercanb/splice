from splice.stdlib import copy, hotpath


@hotpath
def copies_a_list(a: list[int]) -> list[int]:
    return copy(a)
