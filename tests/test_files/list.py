from splice.stdlib import copy


def give_list(l: list[int]) -> list[int]:
    print(l)
    l.append(2)
    print(l)
    return copy(l)


def main() -> int:
    print(list([1, 2, 3]))
    l = [1, 2, 3]
    print(l)
    l = [1, 2, 3]
    print(l)
    l.append(4)
    print(l)
    l = give_list(l)
    print(l)

    print(l[1])
    a = l[0]
    print(l)
    l[0] = a
    print(l)
    l[0] = 2
    print(l)

    l2 = copy(l[0:1])
    print(l2)

    l.insert(0, 100)
    print(l)
    l.insert(2, 200)
    print(l)
    l.insert(-1, 300)
    print(l)
    l.insert(100, 400)
    print(l)
    l.insert(-100, 500)
    print(l)

    l.remove(200)
    print(l)

    x = l.pop()
    print(x, l)
    y = l.pop(0)
    print(y, l)
    z = l.pop(-2)
    print(z, l)

    l.extend([7, 8])
    print(l)

    l3 = l.copy()
    print(l3)

    l.clear()
    print(l)

    l2 = [5, 3, 1, 3, 9]
    print(l2.index(3))
    print(l2.index(3, 3))
    print(l2.index(3, 0, 2))
    print(l2.index(9, -2))
    print(l2.count(3))
    print(l2.count(42))

    l2.sort()
    print(l2)
    l2.sort(reverse=True)
    print(l2)
    l2.sort(reverse=False)
    print(l2)

    print(sorted(l2, key=lambda x: -x))
    print(sorted(l2, key=lambda x: -x, reverse=True))

    l2.sort(key=lambda x: -x)
    print(l2)
    l2.sort(key=lambda x: -x, reverse=True)
    print(l2)

    l2.reverse()
    print(l2)

    n = len(l2)
    print(n)
    print(l2[0], l2[-1])
    assert l2[-1] == l2[n - 1]

    l4 = [1, 2, 3]
    print(l4[-1])
    l4[-1] += 10
    assert l4[-1] == 13
    print(l4)

    l5 = [1, 2] * 3
    print(l5)
    l5 = 3 * [1, 2]
    print(l5)
    assert l5 == [1, 2] * 3
    l5 *= 2
    print(l5)

    return 0
