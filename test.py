from dataclasses import dataclass


@dataclass
class Pair:
    x: int
    y: int


def mutate(p: Pair):
    p.x += p.y


mutate(Pair(10, 20))
