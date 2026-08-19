from splice.stdlib import copy


def main() -> int:
    l = [0, 1, 2, 3, 4, 5]

    print(l[0:1])
    print(l[1:4])
    print(l[:3])
    print(l[3:])
    print(l[:])

    print(l[::2])
    print(l[1:5:2])
    print(l[::3])

    print(l[-3:])
    print(l[:-2])
    print(l[-4:-1])
    print(l[-1:])

    print(l[::-1])
    print(l[4:1:-1])
    print(l[::-2])
    print(l[-1:-4:-1])

    # out of range bounds clamp, they do not raise
    print(l[10:20])
    print(l[-100:100])
    print(l[:100])
    print(l[-100:])

    # empty results
    print(l[2:2])
    print(l[4:1])
    print(l[1:4:-1])

    empty: list[int] = []
    print(empty[:])
    print(empty[0:5])
    print(empty[::-1])

    # a slice is a copy: mutating it leaves the original alone
    original = [1, 2, 3]
    copied = copy(original[:])
    copied.append(4)
    print(original, copied)

    s = "abcdef"

    print(s[0:1])
    print(s[1:4])
    print(s[:3])
    print(s[3:])
    print(s[:])

    print(s[::2])
    print(s[1:5:2])

    print(s[-3:])
    print(s[:-2])
    print(s[-4:-1])

    print(s[::-1])
    print(s[4:1:-1])
    print(s[::-2])

    print(s[100:200])
    print(s[-100:100])
    print(s[2:2])
    print(s[4:1])

    print(""[:])
    print(""[0:5])

    return 0
