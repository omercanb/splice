def main() -> int:
    # Sets are unordered, so order-dependent output goes through sorted().
    s = {3, 1, 2, 1}
    print(len(s))
    print(sorted(s))
    print(sorted(s, reverse=True))
    print(sorted(s, key=lambda x: -x))
    print(sorted(s, key=lambda x: -x, reverse=True))

    s.add(4)
    print(sorted(s))
    s.add(4)
    print(len(s))

    s.remove(4)
    print(sorted(s))
    s.discard(99)
    print(sorted(s))

    a = {1, 2, 3}
    b = {3, 4}
    print(sorted(a | b))
    print(sorted(a & b))
    print(sorted(a - b))
    print(sorted(a ^ b))
    print(sorted(a.union(b)))
    print(sorted(a.intersection(b)))
    print(sorted(a.difference(b)))
    print(sorted(a.symmetric_difference(b)))
    print(sorted(a), sorted(b))

    small = {1, 2}
    big = {1, 2, 3}
    print(small.issubset(big))
    print(big.issuperset(small))
    nine = {9}
    print(small.isdisjoint(nine))
    print(small.isdisjoint(big))
    print(small <= big, small < big)
    print(big >= small, big > small)
    print({1, 2} == {2, 1})
    print(small != big)

    c = a.copy()
    c.add(99)
    print(sorted(a), sorted(c))

    d = {1, 2, 3}
    d.update({4})
    print(sorted(d))
    keep = {2, 3, 4}
    d.intersection_update(keep)
    print(sorted(d))
    drop = {4}
    d.difference_update(drop)
    print(sorted(d))
    d.symmetric_difference_update({3, 5})
    print(sorted(d))

    d.clear()
    print(len(d))

    total = 0
    for x in {1, 2, 3}:
        total = total + x
    print(total)

    s1 = {(1, 2), (1, 3)}
    print(sorted(s1))
    s2 = {(1, 2), (1, 2)}
    print(sorted(s2))
    s3 = {"Hello", "World"}
    print(sorted(s3))

    return 0
