class NoStrClass:
    def __init__(self, x: int = 0) -> None:
        self.x = x


def main() -> int:
    entry = NoStrClass(5)
    print(entry)
    return 0
