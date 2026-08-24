def reused_loop_variable() -> None:
    for i in range(10):
        print(i)
    for i in range(10):
        print(i)
