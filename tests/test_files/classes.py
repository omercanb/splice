class Counter:
    """A class needs no attribute annotations; __init__ is enough."""

    def __init__(self, start: int) -> None:
        self.count = start

    def bump(self, by: int) -> None:
        self.count = self.count + by

    def doubled(self) -> int:
        return self.count * 2


class Point:
    x: int
    y: int

    def __init__(self, x: int, y: int) -> None:
        self.x = x
        self.y = y

    def norm(self) -> int:
        return self.x * self.x + self.y * self.y

    def moved(self, dx: int, dy: int) -> "Point":
        return Point(self.x + dx, self.y + dy)

    # The runtime dispatches to these by name, so they need no special casing.
    def __str__(self) -> str:
        return "Point(" + str(self.x) + ", " + str(self.y) + ")"

    def __len__(self) -> int:
        return 2

    def __bool__(self) -> bool:
        return self.x != 0 or self.y != 0


class Empty:
    pass


def main() -> int:
    counter = Counter(5)
    counter.bump(3)
    print(counter.count, counter.doubled())

    p = Point(3, 4)
    print(p.x, p.y, p.norm())

    # dunders reached through the ordinary builtins
    print(p)
    print(len(p))
    print(bool(p), bool(Point(0, 0)))

    # a method returning a new instance
    q = p.moved(1, 1)
    print(q, q.norm())

    # attributes are ordinary lvalues
    p.x = 10
    print(p.x, p.norm())

    # instances live in containers like anything else
    points = [Point(1, 1), Point(2, 2)]
    for point in points:
        print(point, point.norm())
    print(len(points))

    e = Empty()

    return 0
