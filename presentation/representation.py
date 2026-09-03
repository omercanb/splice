from typing import Literal

from splice.stdlib import Array, int32, uint8


class Token:
    count: int
    payload: Array[uint8, Literal[56]]


class Order:
    ask: int32
    token: Token


"""
Python Representation of Order
Order {
    ask -> pointer(int32)
    token -> pointer(Token) {
        count -> pointer(int)
        payload -> pointer(Array)
    }
}

Splice Representation (Like C++)
Order {
    ask: int32
    # Token
    count: int
    payload: Array[uint8, 56]
}

(Can read directly from hardware buffer)
"""
