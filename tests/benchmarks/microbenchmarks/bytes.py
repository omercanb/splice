from tests.benchmarks.benchmarking import benchmark

_DIGIT_BYTES = [b"0", b"1", b"2", b"3", b"4", b"5", b"6", b"7", b"8", b"9"]


def _int_bytes(n: int) -> bytes:
    """n's decimal digits as bytes - no %-formatting or str<->bytes conversion."""
    if n == 0:
        return b"0"
    out = b""
    while n > 0:
        out = _DIGIT_BYTES[n % 10] + out
        n = n // 10
    return out


@benchmark()
def bytes_concat() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(b'  ' + _int_bytes(i) + b' str')

    n = 0
    for i in range(1000):
        for s in a:
            b = b'foo' + s
            if b == s:
                n += 1
            b += b'bar'
            if b != s:
                n += 1
    assert n == 2000000, n


@benchmark()
def bytes_methods() -> None:
    """Use a mix of bytes methods (but not split/join)."""
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(b'  ' + _int_bytes(i) + b' str')

    n = 0
    for i in range(100):
        for s in a:
            if s.startswith(b'foo'):
                n += 1
            if s.endswith(b'r'):
                n += 1
            if s.replace(b'-', b'/') != s:
                n += 1
            if s.strip() != s:
                n += 1
            if s.rstrip() != s:
                n += 1
            if s.lower() == s:
                n += 1
    assert n == 400000, n


@benchmark()
def bytes_format() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(_int_bytes(i) + b' str')

    n = 0
    for i in range(100):
        for s in a:
            n += len(b"foobar " + s + b" stuff")
            ss = b"foobar " + s + b" stuff"
            n += len(s + b"-" + ss)
    assert n == 10434000, n


@benchmark()
def bytes_slicing() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(_int_bytes(i) + b' str')

    n = 0
    for i in range(1000):
        for s in a:
            n += len(s[2:-2])
            if s[:3] == b'Foo':
                n += 1
            if s[-2:] == b'00':
                n += 1
    assert n == 9789000, n


@benchmark()
def bytes_split_and_join() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(_int_bytes(i) + b'-ab-asdfsdf-asdf')
        a.append(b'yeah')
    n = 0
    for i in range(100):
        for s in a:
            items = s.split(b'-')
            if b'-'.join(items) == s:
                n += 1
    assert n == 300000, n


@benchmark()
def bytes_searching() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(_int_bytes(i) + b'-ab-asdfsdf-asdf')
        a.append(b'yeah')
    n = 0
    for i in range(100):
        for s in a:
            if b'i' in s:
                n += 1
            if s.find(b'asd') >= 0:
                n += 1
            n += s.index(b'a')
    assert n == 1089000, n


@benchmark()
def bytes_call() -> None:
    a = []
    for i in range(100):
        a.append([65, 55])
        a.append([0, 1, 2, 3])
        a.append([100])

    n = 0
    for i in range(10 * 1000):
        for s in a:
            b = bytes(s)
            n += len(b)

    assert n == 7000000, n


@benchmark()
def bytes_indexing() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar-' + _int_bytes(i))
        a.append(_int_bytes(i) + b'-ab-asdfsdf-asdf')
        a.append(b'yeah')
    n = 0
    for i in range(100):
        for s in a:
            for j in range(len(s)):
                if s[j] == 97:
                    n += 1
    assert n == 500000, n


@benchmark()
def bytes_normalize() -> None:
    a = []
    for i in range(1000):
        a.append(b'Foobar  ' + _int_bytes(i))
        a.append(_int_bytes(i) + b'-ab\tasdfsdf-asdf\n')
        a.append(b'yeah')
    n = 0
    for i in range(1000):
        for s in a:
            n += len(b' '.join(filter(None, s.replace(b'\t', b' ').split(b' '))))
    assert n == 33780000, n
