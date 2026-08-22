# Splice
A library to compile statically typed Python programs to standalone C++ for latency critical code.

```python
from splice.stdlib import copy, hotpath


def product(a: list[float], b: list[float]) -> None:
    assert len(a) == len(b)
    for i, b_num in enumerate(b):
        a[i] *= b_num


@hotpath
def main() -> int:
    list1: list[float] = [1, 2, 3, 4, 5]
    list2 = copy(list1)
    for i in range(100):
        product(list1, list2)
    print(list1)
    print(sum(list1))
    return 0
```

Compiles to:

```cpp
#include "runtime.h"
using namespace py;

void product(list<double> &RESTRICT a, const list<double> &RESTRICT b);
FLATTEN int main();

void product(list<double> &RESTRICT a, const list<double> &RESTRICT b) {
    int64_t i;
    double b_num;
    if (!(((len(a) == len(b))))) throw AssertionError("");
    auto && __range_0 = enumerate(b);
    for (auto &&__item_0 : __range_0) {
        destructure(i, b_num) = __item_0;
        a[i] *= b_num;
    }
}

FLATTEN int main() {
    list<double> list1;
    list<double> list2;
    int64_t i;
    list1 = list<double>({1LL, 2LL, 3LL, 4LL, 5LL});
    list2 = copy(list1);
    int64_t __stop_0 = 100LL;
    for (i = 0; i < __stop_0; ++i) {
        product(list1, list2);
    }
    print(list1);
    print(sum(list1));
    return 0LL;
}
```
> **_NOTE:_**  Function parameters are analyzed as const vs non-const

And the compilation gives a warning:

```
input.py:12:26: warning: `main` is @hotpath but allocates here

  12 |     list1: list[float] = [1, 2, 3, 4, 5]
     |                          ^^^^^^^^^^^^^^^

  help: allocation will be too slow for a hot path, please consider another option

```

## Documentation

- [Why It's Fast](docs/value_semantics.md) - how Splice compiles to plain values instead of pointers
- [Architecture](docs/architecture.md) - how compilation works
