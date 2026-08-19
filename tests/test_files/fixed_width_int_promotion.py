from splice.stdlib import int32, uint16, uint8


def scale(value: uint16, factor: uint16) -> uint16:
    return value * factor


def clamp(value: int32, low: int32, high: int32) -> int32:
    if value < low:
        return low
    if value > high:
        return high
    return value


def main() -> int:
    # A bare literal assigns directly - no explicit uint16(...) needed.
    n: uint16 = 10
    m: uint8 = 200
    print(n, m)

    # A bare literal argument works too.
    doubled = scale(n, 2)
    print(doubled)

    # Arithmetic between two already-typed values stays that width,
    # instead of widening to plain int.
    total: uint16 = scale(n, 2) + n
    print(total)

    print(clamp(500, 0, 100))
    print(clamp(-5, 0, 100))
    print(clamp(50, 0, 100))

    return 0
