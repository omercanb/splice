def main() -> int:
    d = {1: 10, 2: 20, 3: 30}

    # Iteration order is unspecified, so order-dependent output goes
    # through sorted().
    print(len(d))
    print(d[1], d[2], d[3])
    print(sorted(d))
    print(sorted(d, reverse=True))
    print(sorted(d, key=lambda k: -k))
    print(sorted(d, key=lambda k: -k, reverse=True))

    d[4] = 40
    print(len(d), d[4])
    d[1] = 11
    print(len(d), d[1])

    print(d.get(1))
    print(d.get(99, -1))

    print(d.pop(4))
    print(d.pop(99, -1))
    print(len(d))

    print(d.setdefault(2, 999))
    print(d.setdefault(9, 90))
    print(sorted(d))

    print(sorted(d.keys()))
    print(sorted(d.values()))

    e = {5: 50}
    d.update(e)
    print(sorted(d))

    c = d.copy()
    print(len(c))
    c.clear()
    print(len(c), len(d))

    s = {"b": 2, "a": 1}
    print(sorted(s))
    print(s["a"], s["b"])

    return 0
