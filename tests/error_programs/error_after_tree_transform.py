from splice.stdlib import _error_after_tree_transform as marker


def f(a: list[int]) -> None:
    a[marker(0)] = 1
