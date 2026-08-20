from typing import Final, Generic, Literal, TypeVar, get_args, reveal_type

from splice.stdlib import Array, uint8, uint16, uint32

numInstruments: Final = 3
numLiteral = Literal[5]
reveal_type(numInstruments)
b = Array[int, numLiteral]
a = Array[int, numInstruments]()
print(a)
print(b)
b = a
