from splice.stdlib import hotpath


def clamp(x: int, low: int, high: int) -> int:
    if x < low:
        return low
    if x > high:
        return high
    return x


def normalize_price(raw: int, tick_size: int) -> int:
    # Rounds down to the nearest tick, then clamps to a sane range - a
    # plain helper, not marked hotpath itself.
    rounded = (raw // tick_size) * tick_size
    return clamp(rounded, 0, 1000000)


@hotpath
def process_order(bid: int, ask: int, qty: int, tick_size: int) -> int:
    # The real use case: one long hotpath function calling into a 2-level
    # chain of plain functions (normalize_price -> clamp), neither marked
    # hotpath themselves - comfortably flattened into this one regardless.
    norm_bid = normalize_price(bid, tick_size)
    norm_ask = normalize_price(ask, tick_size)

    spread = norm_ask - norm_bid
    if spread < 0:
        spread = 0

    total = spread * qty
    if qty > 0:
        total = total + qty
    else:
        total = total - qty

    return total


def square(x: int) -> int:
    return x * x


def add(a: int, b: int) -> int:
    return a + b


@hotpath
def distance_squared(x1: int, y1: int, x2: int, y2: int) -> int:
    dx = square(x2 - x1)
    dy = square(y2 - y1)
    return add(dx, dy)


@hotpath
def sum_up_to(n: int) -> int:
    total = 0
    for i in range(n):
        total = add(total, i)
    return total


class Vector:
    def __init__(self, x: int, y: int) -> None:
        self.x = x
        self.y = y

    def scaled(self, factor: int) -> "Vector":
        return Vector(self.x * factor, self.y * factor)

    @hotpath
    def length_squared(self) -> int:
        # Nested call from a hotpath method into a free function.
        return distance_squared(0, 0, self.x, self.y)

    @hotpath
    def move(self, dx: int, dy: int) -> None:
        # Nested call from one hotpath method into another.
        self.x = clamp(self.x + dx, -100, 100)
        self.y = clamp(self.y + dy, -100, 100)


def main() -> int:
    # Test 1: one long hotpath function, calling a 2-level chain of plain
    # (non-hotpath) helper functions - the actual flatten use case.
    print("Test 1 - process_order:", process_order(10007, 10023, 5, 5))
    print("Test 1b - process_order (sell):", process_order(9998, 10001, -3, 5))

    # Test 2: hotpath function with nested calls to plain functions
    print("Test 2 - distance_squared:", distance_squared(0, 0, 3, 4))

    # Test 3: hotpath function with a loop calling a plain function
    print("Test 3 - sum_up_to:", sum_up_to(5))

    # Test 4: hotpath method calling a hotpath free function
    v = Vector(3, 4)
    print("Test 4 - length_squared:", v.length_squared())

    # Test 5: hotpath method calling another hotpath method
    v.move(200, -200)
    print("Test 5 - move (clamped):", v.x, v.y)

    # Test 6: a non-hotpath method built on the same class, for contrast
    scaled = v.scaled(2)
    print("Test 6 - scaled:", scaled.x, scaled.y)

    return 0
