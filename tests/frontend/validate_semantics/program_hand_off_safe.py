from splice.stdlib import copy


def return_local(a: list[int]) -> list[int]:
    b: list[int] = [x for x in a]
    return b


def return_param_copy(a: list[int]) -> list[int]:
    return copy(a)


def assign_param_copy(a: list[int]) -> list[int]:
    b = copy(a)
    return b


def return_scalar_field(a: list[int]) -> int:
    return a[0]


def assign_scalar(a: list[int]) -> int:
    x = a[0]
    return x


def return_literal() -> list[int]:
    return [1, 2, 3]


def make() -> list[int]:
    return [1, 2, 3]


def return_call_result() -> list[int]:
    return make()


def return_slice_of_param(a: list[int]) -> list[int]:
    return a[0:1]
