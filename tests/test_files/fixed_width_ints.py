from splice.stdlib import int8, int32, int64, uint8, uint16, uint32, uint64


def add_u16(a: uint16, b: uint16) -> uint16:
    return uint16(a + b)


def main() -> int:
    a: uint8 = uint8(200)
    b: int8 = int8(-100)
    print(a, b)

    c: uint16 = uint16(1000)
    d: uint16 = add_u16(c, uint16(234))
    print(d)

    e: int32 = int32(-70000)
    f: uint32 = uint32(70000)
    print(e, f)

    g: int64 = int64(-5000000000)
    h: uint64 = uint64(5000000000)
    print(g, h)

    print(uint16(5) == uint16(5))
    print(uint16(5) < uint16(10))

    values: list[uint16] = [uint16(3), uint16(1), uint16(2)]
    print(values)

    counts: dict[uint8, str] = {}
    counts[uint8(1)] = "one"
    counts[uint8(2)] = "two"
    print(counts[uint8(1)], counts[uint8(2)])

    seen: set[int32] = {int32(1), int32(2), int32(2)}
    print(len(seen))

    return 0
