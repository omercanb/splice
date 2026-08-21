from typing import Final

from splice.stdlib import Array, uint8, uint32

numCores: Final = 21
numInstruments: Final = 3
# An expression of other constexpr globals, not just a bare literal.
totalSlots: Final = numCores + numInstruments

# Fixed-width int globals, matching real FPGA-style masks: combined via
# shifts/or, not used as an array size at all.
spot: Final[uint8] = 0
future1: Final[uint8] = 1
future2: Final[uint8] = 2
pair0_mask: Final[uint32] = (1 << future1) | (1 << spot)
pair2_mask: Final[uint32] = (1 << future1) | (1 << future2)


def main() -> int:
    # Test 1: a derived global (expression of other globals)
    print("Test 1 - derived global:", totalSlots)

    # Test 2: that derived global used as an Array size
    arr = Array[uint8, totalSlots]()
    print("Test 2 - array sized by derived global:", len(arr))

    # Test 3: fixed-width int globals combined via shift/or
    print("Test 3 - shift masks:", pair0_mask, pair2_mask)

    return 0
