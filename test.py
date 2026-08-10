from dataclasses import dataclass


@dataclass
class Pair:
    x: int
    y: int


def mutate(p: Pair):
    p.x += p.y


mutate(Pair(10, 20))

items = [1, 2, 3]

for x in items:
    print(items)
    items.append(x)
