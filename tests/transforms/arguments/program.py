def f(a: int, b: int = 2, c: int = 3) -> int:
    return a + b + c


class Box:
    def __init__(self, value: int) -> None:
        self.value = value

    def add(self, n: int, times: int = 2) -> int:
        return self.value + n * times


def main() -> int:
    fully_positional = f(1, 2, 3)
    omits_trailing_defaults = f(1)
    one_keyword = f(1, c=5)
    keywords_out_of_order = f(c=5, a=1)
    box = Box(10)
    unrewritten_method_call = box.add(3)
    return 0
