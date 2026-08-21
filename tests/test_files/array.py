from typing import Final, Literal

from splice.stdlib import Array, uint32

numInstruments: Final = 3


def main() -> int:
    # Test 1: construct with a Final int size, default-initialized (all zero)
    a = Array[uint32, numInstruments]()
    a[1] = 5
    print("Test 1 - index/assign:", a[0], a[1], a[2])

    # Test 2: construct with an explicit Literal size
    b = Array[uint32, Literal[4]]()
    print("Test 2 - Literal size:", b[0], b[3])

    # Test 3: -1 reads the last element
    print("Test 3 - last element:", a[-1])

    # Test 4: len()
    print("Test 4 - len:", len(a), len(b))

    # Test 5: fill()
    b.fill(uint32(1))
    print("Test 5 - fill:", b[0], b[1], b[2], b[3])

    # Test 6: iteration
    total = 0
    for v in b:
        total = total + v
    print("Test 6 - iteration sum:", total)

    # Test 7: whole-array printing
    print("Test 7 - str:", a)

    return 0
