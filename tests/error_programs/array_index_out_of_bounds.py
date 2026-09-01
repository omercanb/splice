from typing import Final

from splice.stdlib import Array

N: Final = 3


def main() -> int:
    a = Array[int, N]()
    return a[N]
