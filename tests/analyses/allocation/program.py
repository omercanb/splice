def allocates_directly(a: int, b: int) -> int:
    c = [a, b]
    return c[0] + c[1]


def calls_allocating(a: int, b: int) -> int:
    return allocates_directly(a, b)


def calls_transitively(a: int, b: int) -> int:
    return calls_allocating(a, b)


def pure(a: int, b: int) -> int:
    return a + b


def recursive(n: int) -> int:
    if n <= 1:
        return 1
    return n * recursive(n - 1)


def main() -> int:
    x = pure(1, 2)
    y = calls_transitively(1, 2)
    z = recursive(5)
    return x + y + z
