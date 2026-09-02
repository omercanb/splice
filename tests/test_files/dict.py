def main() -> int:
    d = {1: 10, 2: 20, 3: 30}

    # dict is insertion-ordered here (unordered_dense), matching CPython, as
    # long as nothing's been erased - erasing does swap-and-pop rather than
    # CPython's tombstone-and-compact, so it only stays order-preserving if
    # whatever's erased was the most-recently-inserted survivor at the time
    # (true below: d.pop(4) removes the last key added). Don't rely on this
    # in general - .keys()/.values() below still need sorted() regardless,
    # since they come back as a plain list rather than a dict_view.
    print(len(d))
    print(d[1], d[2], d[3])
    print(d)
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
    print(d)

    print(sorted(d.keys()))
    print(sorted(d.values()))

    e = {5: 50}
    d.update(e)
    print(d)

    c = d.copy()
    print(len(c))
    c.clear()
    print(len(c), len(d))

    s = {"b": 2, "a": 1}
    print(s)
    print(s["a"], s["b"])

    return 0
