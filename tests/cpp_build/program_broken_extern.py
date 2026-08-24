from extern_mod import broken_function


def main() -> int:
    a = broken_function(5)
    b = broken_function(10)
    print(a, b)
    return 0
