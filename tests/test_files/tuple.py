from splice.stdlib import copy


def main() -> int:
    # Test 1: Simple tuple destructuring
    a, b = (1, 2)
    print("Test 1 - Simple destructure:", a, b)

    # Test 2: Reassigning with destructuring
    a, b = (10, 20)
    print("Test 2 - Reassign:", a, b)

    # Test 3: Destructuring with different types
    x, s = (42, "hello")
    print("Test 3 - Mixed types:", x, s)

    # Test 4: Multiple destructures in sequence
    p, q = (100, 200)
    m, n = (p, q)
    print("Test 4 - Chained destructure:", m, n)

    t = (1, 2)
    t2 = copy(t)

    z = t[0]
    z = t[1]

    return 0
