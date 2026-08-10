from splice.stdlib import _error_after_tree_transform as marker


def f(xs: list[int]) -> list[int]:
    return [marker(x) for x in xs]
