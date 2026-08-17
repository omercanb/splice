class C:
    items: list[int]

    def __init__(self, x: int) -> None:
        self.items = [x]

    def bye(self) -> None:
        print("bye")

    def add(self, x: int) -> None:
        self.items.append(x)


def hi() -> None:
    print("hi")


def add(a: int, b: int) -> int:
    return a + b


def mul(a: int, b: int = 2) -> int:
    return a * b


def main() -> int:
    a: list[int] = list()
    i = ord("a")
    print(i)
    hi()
    a.append(i)
    c = C(i)
    c.bye()
    c.add(i)

    num1 = 10
    num2 = 20
    num3 = add(num1, num2)

    mul(num1)
    mul(num2, num3)

    return 0
