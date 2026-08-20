def outer() -> int:
    y = 1

    def inner() -> None:
        nonlocal y
        y = 2

    inner()
    return y
