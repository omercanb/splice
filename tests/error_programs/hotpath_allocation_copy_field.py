from splice.stdlib import copy, hotpath


class HasList:
    def __init__(self) -> None:
        self.items: list[int] = []


@hotpath
def copies_a_class_with_a_list_field(h: HasList) -> HasList:
    return copy(h)
