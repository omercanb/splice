class C:
    items: list[int]

    def bye(self) -> None:
        print("bye")

    def add(self, x: int) -> None:
        self.items.append(x)


def hi() -> None:
    print("hi")


def main() -> int:
    a: list[int] = list()
    i = ord("a")
    print(i)
    hi()
    a.append(i)
    c = C()
    c.bye()
    c.add(i)
    return 0
