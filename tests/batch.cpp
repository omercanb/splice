#include "runtime.h"
using namespace py;

namespace prog_array {
constexpr int64_t numInstruments = 3LL;

int run();

int run() {
    Array<uint32_t, 3> a;
    Array<uint32_t, 4> b;
    int64_t total;
    uint32_t v;
    a = Array<uint32_t, 3>();
    a[1LL] = 5LL;
    print(tuple(str("Test 1 - index/assign:"), a[0LL], a[1LL], a[2LL]), str(" "), str("\n"));
    b = Array<uint32_t, 4>();
    print(tuple(str("Test 2 - Literal size:"), b[0LL], b[3LL]), str(" "), str("\n"));
    print(tuple(str("Test 3 - last element:"), a.back()), str(" "), str("\n"));
    print(tuple(str("Test 4 - len:"), len(a), len(b)), str(" "), str("\n"));
    b.fill(uint32_t(1LL));
    print(tuple(str("Test 5 - fill:"), b[0LL], b[1LL], b[2LL], b[3LL]), str(" "), str("\n"));
    total = 0LL;
    auto && __range_0 = b;
    for (auto &&__item_0 : __range_0) {
        v = __item_0;
        total = (total + v);
    }
    print(tuple(str("Test 6 - iteration sum:"), total), str(" "), str("\n"));
    print(tuple(str("Test 7 - str:"), a), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_boolops {

int64_t side(const int64_t &RESTRICT v);
int run();

int64_t side(const int64_t &RESTRICT v) {
    print(tuple(str("SIDE")), str(" "), str("\n"));
    return v;
}

int run() {
    int64_t a;
    int64_t b;
    int64_t zero;
    str empty;
    str text;
    list<int64_t> no_items;
    double f;
    double g;
    int64_t n;
    a = 1LL;
    b = 2LL;
    zero = 0LL;
    print(tuple(_and(a, b)), str(" "), str("\n"));
    print(tuple(_or(a, b)), str(" "), str("\n"));
    print(tuple(_and(zero, b)), str(" "), str("\n"));
    print(tuple(_or(zero, b)), str(" "), str("\n"));
    print(tuple(_and(zero, zero)), str(" "), str("\n"));
    print(tuple(_or(zero, zero)), str(" "), str("\n"));
    empty = str("");
    text = str("hi");
    print(tuple(_or(empty, text)), str(" "), str("\n"));
    print(tuple(_and(text, empty)), str(" "), str("\n"));
    print(tuple(_or(text, empty)), str(" "), str("\n"));
    print(tuple(_and(empty, text)), str(" "), str("\n"));
    no_items = list<int64_t>();
    print(tuple(_or(no_items, list<int64_t>({1LL, 2LL}))), str(" "), str("\n"));
    print(tuple(_and(list<int64_t>({3LL}), no_items)), str(" "), str("\n"));
    print(tuple(_or(list<int64_t>({3LL}), list<int64_t>({4LL}))), str(" "), str("\n"));
    print(tuple(_and(no_items, list<int64_t>({4LL}))), str(" "), str("\n"));
    f = 0.0;
    g = 2.5;
    print(tuple(_or(f, g)), str(" "), str("\n"));
    print(tuple(_and(g, f)), str(" "), str("\n"));
    print(tuple(_and(zero, side(9LL))), str(" "), str("\n"));
    print(tuple(_or(a, side(9LL))), str(" "), str("\n"));
    print(tuple(_and(a, side(9LL))), str(" "), str("\n"));
    print(tuple(_or(zero, side(9LL))), str(" "), str("\n"));
    print(tuple(_and(a, _and(b, 3LL))), str(" "), str("\n"));
    print(tuple(_or(zero, _or(zero, 3LL))), str(" "), str("\n"));
    print(tuple(_or(_and(a, b), 3LL)), str(" "), str("\n"));
    print(tuple(_or(zero, _and(b, 3LL))), str(" "), str("\n"));
    print(tuple((_and(a, b) + 1LL)), str(" "), str("\n"));
    if ((a && to_bool(text))) {
        print(tuple(str("cond and")), str(" "), str("\n"));
    }
    if ((to_bool(empty) || a)) {
        print(tuple(str("cond or")), str(" "), str("\n"));
    }
    if ((!(to_bool(empty) || zero))) {
        print(tuple(str("cond not")), str(" "), str("\n"));
    }
    if ((a && (to_bool(text) && b))) {
        print(tuple(str("cond chained")), str(" "), str("\n"));
    }
    if ((to_bool(no_items) || to_bool(text))) {
        print(tuple(str("cond mixed")), str(" "), str("\n"));
    }
    if ((zero && side(9LL))) {
        print(tuple(str("unreachable")), str(" "), str("\n"));
    }
    n = 0LL;
    while ((((n < 2LL)) && to_bool(text))) {
        n = (n + 1LL);
    }
    print(tuple(n), str(" "), str("\n"));
    print(tuple((!to_bool(a))), str(" "), str("\n"));
    print(tuple((!to_bool(zero))), str(" "), str("\n"));
    print(tuple((!to_bool(empty))), str(" "), str("\n"));
    print(tuple((!to_bool(_and(a, b)))), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_builtin_functions {

list<int64_t> __list_comprehension_0(const list<int64_t> &RESTRICT numbers);
int run();

list<int64_t> __list_comprehension_0(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_0;
    int64_t v;
    __tmp_0 = list<int64_t>();
    auto && __range_1 = numbers;
    for (auto &&__item_1 : __range_1) {
        v = __item_1;
        __tmp_0.append((v * 2LL));
    }
    return __tmp_0;
}

int run() {
    list<int64_t> numbers;
    list<int64_t> empty;
    list<int64_t> zeros;
    list<int64_t> with_zero;
    numbers = list<int64_t>({4LL, 1LL, 7LL, 3LL});
    empty = list<int64_t>();
    print(tuple(sum(numbers), sum(empty)), str(" "), str("\n"));
    print(tuple(min(numbers), max(numbers)), str(" "), str("\n"));
    print(tuple(min(3LL, 8LL), max(3LL, 8LL), min(2.5, 1.5)), str(" "), str("\n"));
    zeros = list<int64_t>({0LL, 0LL});
    with_zero = list<int64_t>({1LL, 0LL, 2LL});
    print(tuple(any(numbers), any(zeros), any(empty)), str(" "), str("\n"));
    print(tuple(all(numbers), all(with_zero), all(empty)), str(" "), str("\n"));
    print(tuple(sum(range(5LL)), max(range(5LL))), str(" "), str("\n"));
    print(tuple(sum(set<int64_t>({1LL, 2LL, 3LL}))), str(" "), str("\n"));
    print(tuple(min(str("hello")), max(str("hello"))), str(" "), str("\n"));
    print(tuple(divmod(7LL, 2LL), divmod((-7LL), 2LL)), str(" "), str("\n"));
    print(tuple(py::round(2.5), py::round(3.5), py::round((-2.5))), str(" "), str("\n"));
    print(tuple(py::round(2.567, 1LL), py::round(2.567, 2LL)), str(" "), str("\n"));
    print(tuple(py::round(5LL)), str(" "), str("\n"));
    print(tuple(chr(65LL), 65LL), str(" "), str("\n"));
    print(tuple(chr((97LL + 1LL))), str(" "), str("\n"));
    print(tuple(sum(__list_comprehension_0(numbers))), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_bytes {

int run();

int run() {
    bytes b;
    bytes padded;
    bytes a;
    bytes c;
    int64_t total;
    int64_t byte;
    bytes joined;
    b = bytes(std::string("Hello World", 11));
    print(tuple(b), str(" "), str("\n"));
    print(tuple(len(b)), str(" "), str("\n"));
    print(tuple(b[0LL], b.back()), str(" "), str("\n"));
    print(tuple(b.upper()), str(" "), str("\n"));
    print(tuple(b.lower()), str(" "), str("\n"));
    print(tuple(b.swapcase()), str(" "), str("\n"));
    print(tuple(b.capitalize()), str(" "), str("\n"));
    print(tuple(bytes(std::string("hello world", 11)).title()), str(" "), str("\n"));
    print(tuple(b.find(bytes(std::string("o", 1)))), str(" "), str("\n"));
    print(tuple(b.find(bytes(std::string("o", 1)), 5LL)), str(" "), str("\n"));
    print(tuple(b.rfind(bytes(std::string("o", 1)))), str(" "), str("\n"));
    print(tuple(b.find(bytes(std::string("zz", 2)))), str(" "), str("\n"));
    print(tuple(b.index(bytes(std::string("World", 5)))), str(" "), str("\n"));
    print(tuple(b.count(bytes(std::string("l", 1)))), str(" "), str("\n"));
    print(tuple(b.count(bytes(std::string("zz", 2)))), str(" "), str("\n"));
    print(tuple(b.startswith(bytes(std::string("Hello", 5)))), str(" "), str("\n"));
    print(tuple(b.startswith(bytes(std::string("World", 5)))), str(" "), str("\n"));
    print(tuple(b.endswith(bytes(std::string("World", 5)))), str(" "), str("\n"));
    print(tuple(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1)))), str(" "), str("\n"));
    print(tuple(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1)), 2LL)), str(" "), str("\n"));
    print(tuple(b.removeprefix(bytes(std::string("Hello ", 6)))), str(" "), str("\n"));
    print(tuple(b.removesuffix(bytes(std::string(" World", 6)))), str(" "), str("\n"));
    padded = bytes(std::string("  spaced  ", 10));
    print(tuple(padded.strip()), str(" "), str("\n"));
    print(tuple(padded.lstrip()), str(" "), str("\n"));
    print(tuple(padded.rstrip()), str(" "), str("\n"));
    print(tuple(bytes(std::string("xxhixx", 6)).strip(bytes(std::string("x", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("hi", 2)).ljust(5LL, bytes(std::string(".", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("hi", 2)).rjust(5LL, bytes(std::string(".", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("hi", 2)).center(6LL, bytes(std::string(".", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("42", 2)).zfill(5LL)), str(" "), str("\n"));
    print(tuple(bytes(std::string("-42", 3)).zfill(5LL)), str(" "), str("\n"));
    print(tuple(bytes(std::string("abc", 3)).isalpha(), bytes(std::string("a1", 2)).isalpha()), str(" "), str("\n"));
    print(tuple(bytes(std::string("123", 3)).isdigit(), bytes(std::string("12a", 3)).isdigit()), str(" "), str("\n"));
    print(tuple(bytes(std::string("a1", 2)).isalnum(), bytes(std::string("a-1", 3)).isalnum()), str(" "), str("\n"));
    print(tuple(bytes(std::string("  ", 2)).isspace(), bytes(std::string("a ", 2)).isspace()), str(" "), str("\n"));
    print(tuple(bytes(std::string("ABC", 3)).isupper(), bytes(std::string("Abc", 3)).isupper()), str(" "), str("\n"));
    print(tuple(bytes(std::string("abc", 3)).islower(), bytes(std::string("Abc", 3)).islower()), str(" "), str("\n"));
    print(tuple(b.split()), str(" "), str("\n"));
    print(tuple(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("a,,b", 4)).split(bytes(std::string(",", 1)))), str(" "), str("\n"));
    print(tuple(bytes(std::string("-", 1)).join(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1))))), str(" "), str("\n"));
    print(tuple(bytes(std::string("one\ntwo", 7)).splitlines()), str(" "), str("\n"));
    a = bytes(std::string("foo", 3));
    c = bytes(std::string("bar", 3));
    print(tuple((a + c)), str(" "), str("\n"));
    print(tuple((a * 3LL)), str(" "), str("\n"));
    print(tuple(((a == bytes(std::string("foo", 3)))), ((a == c))), str(" "), str("\n"));
    print(tuple(((a < c)), ((a > c))), str(" "), str("\n"));
    print(tuple((b.__contains__(bytes(std::string("World", 5))))), str(" "), str("\n"));
    print(tuple((b.__contains__(87LL))), str(" "), str("\n"));
    print(tuple((b.__contains__(90LL))), str(" "), str("\n"));
    print(tuple(bytes(3LL)), str(" "), str("\n"));
    print(tuple(bytes(list<int64_t>({65LL, 66LL, 67LL}))), str(" "), str("\n"));
    total = 0LL;
    auto && __range_2 = bytes(std::string("abc", 3));
    for (auto &&__item_2 : __range_2) {
        byte = __item_2;
        total += byte;
    }
    print(tuple(total), str(" "), str("\n"));
    joined = bytes(std::string("", 0));
    auto && __range_3 = bytes(std::string("abc", 3));
    for (auto &&__item_3 : __range_3) {
        byte = __item_3;
        joined = ((joined + bytes(list<int64_t>({byte}))) + bytes(std::string(".", 1)));
    }
    print(tuple(joined), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_casts {

int run();

int run() {
    double a;
    int64_t b;
    double c;
    str float_str;
    double f1;
    str int_str;
    int64_t i1;
    int64_t i2;
    a = 2.0;
    b = to_int(a);
    c = to_float(b);
    float_str = str("  0.10 ");
    f1 = to_float(float_str);
    int_str = str("100");
    i1 = to_int(int_str);
    i2 = to_int(int_str, 2LL);
    print(tuple(a, b, c, f1, i1, i2), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_classes {

class Counter;
class Point;
class Empty;

int run();

class Counter {
  public:
    int64_t count;

    Counter(const int64_t &RESTRICT start) { __init__(start); }

    Counter() = default;

    void __init__(const int64_t &RESTRICT start);
    void bump(const int64_t &RESTRICT by);
    int64_t doubled();
};

class Point {
  public:
    int64_t x;
    int64_t y;

    Point(const int64_t &RESTRICT x, const int64_t &RESTRICT y) { __init__(x, y); }

    Point() = default;

    void __init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y);
    int64_t norm();
    Point moved(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy);
    str __str__();
    int64_t __len__();
    bool __bool__();
};

class Empty {
  public:
};

void Counter::__init__(const int64_t &RESTRICT start) {
    this->count = start;
}

void Counter::bump(const int64_t &RESTRICT by) {
    this->count = (this->count + by);
}

int64_t Counter::doubled() {
    return (this->count * 2LL);
}

void Point::__init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y) {
    this->x = x;
    this->y = y;
}

int64_t Point::norm() {
    return ((this->x * this->x) + (this->y * this->y));
}

Point Point::moved(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy) {
    return Point((this->x + dx), (this->y + dy));
}

str Point::__str__() {
    return ((((str("Point(") + to_str(this->x)) + str(", ")) + to_str(this->y)) + str(")"));
}

int64_t Point::__len__() {
    return 2LL;
}

bool Point::__bool__() {
    return _or(((this->x != 0LL)), ((this->y != 0LL)));
}

int run() {
    Counter counter;
    Point p;
    Point q;
    list<Point> points;
    Point point;
    Empty e;
    counter = Counter(5LL);
    counter.bump(3LL);
    print(tuple(counter.count, counter.doubled()), str(" "), str("\n"));
    p = Point(3LL, 4LL);
    print(tuple(p.x, p.y, p.norm()), str(" "), str("\n"));
    print(tuple(p), str(" "), str("\n"));
    print(tuple(len(p)), str(" "), str("\n"));
    print(tuple(to_bool(p), to_bool(Point(0LL, 0LL))), str(" "), str("\n"));
    q = p.moved(1LL, 1LL);
    print(tuple(q, q.norm()), str(" "), str("\n"));
    p.x = 10LL;
    print(tuple(p.x, p.norm()), str(" "), str("\n"));
    points = list<Point>({Point(1LL, 1LL), Point(2LL, 2LL)});
    auto && __range_4 = points;
    for (auto &&__item_4 : __range_4) {
        point = __item_4;
        print(tuple(point, point.norm()), str(" "), str("\n"));
    }
    print(tuple(len(points)), str(" "), str("\n"));
    e = Empty();
    return 0LL;
}
}

namespace prog_comparison {

int run();

int run() {
    int64_t a;
    int64_t b;
    int64_t c;
    int64_t d;
    a = 1LL;
    b = 2LL;
    c = 3LL;
    print(tuple(((a < b) && (b < c))), str(" "), str("\n"));
    print(tuple(((a > b))), str(" "), str("\n"));
    print(tuple(((a > b) && (b > c))), str(" "), str("\n"));
    d = 3LL;
    print(tuple(((c <= d))), str(" "), str("\n"));
    print(tuple(((a == d))), str(" "), str("\n"));
    print(tuple(((c == d))), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_comprehensions {

list<int64_t> __list_comprehension_1(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor, const int64_t &RESTRICT offset);
list<int64_t> scaled(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor);
list<int64_t> __list_comprehension_2(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_3(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_4();
list<int64_t> __list_comprehension_5();
list<int64_t> __list_comprehension_6(const list<int64_t> &RESTRICT numbers);
set<int64_t> __set_comprehension_0(const list<int64_t> &RESTRICT numbers);
dict<int64_t, int64_t> __dict_comprehension_0(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_7(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_11(const int64_t &RESTRICT v);
list<int64_t> __list_comprehension_8(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_9(const list<int64_t> &RESTRICT numbers);
list<int64_t> __list_comprehension_10(const list<int64_t> &RESTRICT doubled);
int run();

list<int64_t> __list_comprehension_1(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor, const int64_t &RESTRICT offset) {
    list<int64_t> __tmp_1;
    int64_t v;
    __tmp_1 = list<int64_t>();
    auto && __range_5 = values;
    for (auto &&__item_5 : __range_5) {
        v = __item_5;
        __tmp_1.append(((v * factor) + offset));
    }
    return __tmp_1;
}

list<int64_t> scaled(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor) {
    int64_t offset;
    offset = 1LL;
    return __list_comprehension_1(values, factor, offset);
}

list<int64_t> __list_comprehension_2(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_2;
    int64_t v;
    __tmp_2 = list<int64_t>();
    auto && __range_6 = numbers;
    for (auto &&__item_6 : __range_6) {
        v = __item_6;
        __tmp_2.append(v);
    }
    return __tmp_2;
}

list<int64_t> __list_comprehension_3(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_3;
    int64_t v;
    __tmp_3 = list<int64_t>();
    auto && __range_7 = numbers;
    for (auto &&__item_7 : __range_7) {
        v = __item_7;
        if (((v > 2LL))) {
            __tmp_3.append(v);
        }
    }
    return __tmp_3;
}

list<int64_t> __list_comprehension_4() {
    list<int64_t> __tmp_4;
    int64_t i;
    __tmp_4 = list<int64_t>();
    int64_t __stop_0 = 4LL;
    for (i = 0; i < __stop_0; ++i) {
        __tmp_4.append(i);
    }
    return __tmp_4;
}

list<int64_t> __list_comprehension_5() {
    list<int64_t> __tmp_5;
    int64_t i;
    __tmp_5 = list<int64_t>();
    int64_t __stop_1 = 8LL;
    for (i = 1LL; i < __stop_1; i += 2) {
        __tmp_5.append(i);
    }
    return __tmp_5;
}

list<int64_t> __list_comprehension_6(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_6;
    int64_t i;
    __tmp_6 = list<int64_t>();
    int64_t __len_0 = len(numbers);
    for (i = 0; i < __len_0; ++i) {
        __tmp_6.append(numbers[i]);
    }
    return __tmp_6;
}

set<int64_t> __set_comprehension_0(const list<int64_t> &RESTRICT numbers) {
    set<int64_t> __tmp_7;
    int64_t v;
    __tmp_7 = set<int64_t>();
    auto && __range_8 = numbers;
    for (auto &&__item_8 : __range_8) {
        v = __item_8;
        __tmp_7.add((v * v));
    }
    return __tmp_7;
}

dict<int64_t, int64_t> __dict_comprehension_0(const list<int64_t> &RESTRICT numbers) {
    dict<int64_t, int64_t> __tmp_8;
    int64_t v;
    __tmp_8 = dict<int64_t, int64_t>();
    auto && __range_9 = numbers;
    for (auto &&__item_9 : __range_9) {
        v = __item_9;
        if (((v > 1LL))) {
            __tmp_8[v] = (v * v);
        }
    }
    return __tmp_8;
}

list<int64_t> __list_comprehension_7(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_9;
    int64_t x;
    int64_t y;
    __tmp_9 = list<int64_t>();
    auto && __range_10 = numbers;
    for (auto &&__item_10 : __range_10) {
        x = __item_10;
        auto && __range_11 = numbers;
        for (auto &&__item_11 : __range_11) {
            y = __item_11;
            if (((x < y))) {
                __tmp_9.append((x * y));
            }
        }
    }
    return __tmp_9;
}

list<int64_t> __list_comprehension_11(const int64_t &RESTRICT v) {
    list<int64_t> __tmp_13;
    int64_t w;
    __tmp_13 = list<int64_t>();
    int64_t __stop_2 = v;
    for (w = 0; w < __stop_2; ++w) {
        __tmp_13.append(w);
    }
    return __tmp_13;
}

list<int64_t> __list_comprehension_8(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_10;
    int64_t v;
    __tmp_10 = list<int64_t>();
    auto && __range_12 = numbers;
    for (auto &&__item_12 : __range_12) {
        v = __item_12;
        __tmp_10.append(len(__list_comprehension_11(v)));
    }
    return __tmp_10;
}

list<int64_t> __list_comprehension_9(const list<int64_t> &RESTRICT numbers) {
    list<int64_t> __tmp_11;
    int64_t v;
    __tmp_11 = list<int64_t>();
    auto && __range_13 = numbers;
    for (auto &&__item_13 : __range_13) {
        v = __item_13;
        __tmp_11.append((v * 2LL));
    }
    return __tmp_11;
}

list<int64_t> __list_comprehension_10(const list<int64_t> &RESTRICT doubled) {
    list<int64_t> __tmp_12;
    int64_t v;
    __tmp_12 = list<int64_t>();
    auto && __range_14 = doubled;
    for (auto &&__item_14 : __range_14) {
        v = __item_14;
        __tmp_12.append((v + 1LL));
    }
    return __tmp_12;
}

int run() {
    list<int64_t> numbers;
    set<int64_t> squares;
    dict<int64_t, int64_t> lookup;
    list<int64_t> doubled;
    numbers = list<int64_t>({1LL, 2LL, 3LL, 4LL});
    print(tuple(__list_comprehension_2(numbers)), str(" "), str("\n"));
    print(tuple(__list_comprehension_3(numbers)), str(" "), str("\n"));
    print(tuple(__list_comprehension_4()), str(" "), str("\n"));
    print(tuple(__list_comprehension_5()), str(" "), str("\n"));
    print(tuple(__list_comprehension_6(numbers)), str(" "), str("\n"));
    print(tuple(scaled(numbers, 10LL)), str(" "), str("\n"));
    squares = __set_comprehension_0(numbers);
    print(tuple(sorted(squares, false)), str(" "), str("\n"));
    lookup = __dict_comprehension_0(numbers);
    print(tuple(len(lookup), lookup[2LL], lookup[4LL]), str(" "), str("\n"));
    print(tuple(sorted(lookup, false)), str(" "), str("\n"));
    print(tuple(__list_comprehension_7(numbers)), str(" "), str("\n"));
    print(tuple(__list_comprehension_8(numbers)), str(" "), str("\n"));
    doubled = __list_comprehension_9(numbers);
    print(tuple(__list_comprehension_10(doubled)), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_dict {

int run();

int run() {
    dict<int64_t, int64_t> d;
    dict<int64_t, int64_t> e;
    dict<int64_t, int64_t> c;
    dict<str, int64_t> s;
    d = dict<int64_t, int64_t>({{1LL, 10LL}, {2LL, 20LL}, {3LL, 30LL}});
    print(tuple(len(d)), str(" "), str("\n"));
    print(tuple(d[1LL], d[2LL], d[3LL]), str(" "), str("\n"));
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    print(tuple(sorted(d, true)), str(" "), str("\n"));
    d[4LL] = 40LL;
    print(tuple(len(d), d[4LL]), str(" "), str("\n"));
    d[1LL] = 11LL;
    print(tuple(len(d), d[1LL]), str(" "), str("\n"));
    print(tuple(d.get(1LL)), str(" "), str("\n"));
    print(tuple(d.get(99LL, (-1LL))), str(" "), str("\n"));
    print(tuple(d.pop(4LL)), str(" "), str("\n"));
    print(tuple(d.pop(99LL, (-1LL))), str(" "), str("\n"));
    print(tuple(len(d)), str(" "), str("\n"));
    print(tuple(d.setdefault(2LL, 999LL)), str(" "), str("\n"));
    print(tuple(d.setdefault(9LL, 90LL)), str(" "), str("\n"));
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    print(tuple(sorted(d.keys(), false)), str(" "), str("\n"));
    print(tuple(sorted(d.values(), false)), str(" "), str("\n"));
    e = dict<int64_t, int64_t>({{5LL, 50LL}});
    d.update(e);
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    c = d.copy();
    print(tuple(len(c)), str(" "), str("\n"));
    c.clear();
    print(tuple(len(c), len(d)), str(" "), str("\n"));
    s = dict<str, int64_t>({{str("b"), 2LL}, {str("a"), 1LL}});
    print(tuple(sorted(s, false)), str(" "), str("\n"));
    print(tuple(s[str("a")], s[str("b")]), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_exceptions {

int64_t guarded_parse(const str &RESTRICT text);
int64_t parse_or(const str &RESTRICT text, const int64_t &RESTRICT fallback);
int64_t nested();
int64_t handler_raises();
int64_t check_positive(const int64_t &RESTRICT n);
int64_t reraise(const int64_t &RESTRICT n);
int64_t raise_bare_class(const str &RESTRICT key);
int64_t siblings(const str &RESTRICT first, const str &RESTRICT second);
int64_t relay();
int run();

int64_t guarded_parse(const str &RESTRICT text) {
    {
        Finally __finally([&] {
            print(tuple(str("cleanup"), text), str(" "), str("\n"));
        });
        return to_int(text);
    }
}

int64_t parse_or(const str &RESTRICT text, const int64_t &RESTRICT fallback) {
    int64_t value;
    {
        bool __thrown = false;
        try {
            value = to_int(text);
        } catch (ValueError &) {
            __thrown = true;
            print(tuple(str("bad literal"), text), str(" "), str("\n"));
            return fallback;
        }
        if (!__thrown) {
            print(tuple(str("good literal"), text), str(" "), str("\n"));
            return value;
        }
    }
}

int64_t nested() {
    {
        Finally __finally([&] {
            print(tuple(str("outer finally")), str(" "), str("\n"));
        });
        try {
            {
                Finally __finally([&] {
                    print(tuple(str("inner finally")), str(" "), str("\n"));
                });
                return to_int(str("nope"));
            }
        } catch (ValueError &) {
            print(tuple(str("outer caught")), str(" "), str("\n"));
            return (-1LL);
        }
    }
}

int64_t handler_raises() {
    try {
        {
            Finally __finally([&] {
                print(tuple(str("guard ran")), str(" "), str("\n"));
            });
            bool __thrown = false;
            try {
                print(tuple(to_int(str("bad"))), str(" "), str("\n"));
            } catch (ValueError &) {
                __thrown = true;
                print(tuple(str("handler raising")), str(" "), str("\n"));
                return to_int(str("worse"));
            }
            if (!__thrown) {
                print(tuple(str("not reached")), str(" "), str("\n"));
            }
        }
    } catch (ValueError &) {
        print(tuple(str("caught the handler's exception")), str(" "), str("\n"));
    }
    return 0LL;
}

int64_t check_positive(const int64_t &RESTRICT n) {
    if (((n < 0LL))) {
        throw ValueError(str("negative"));
    }
    return n;
}

int64_t reraise(const int64_t &RESTRICT n) {
    {
        Finally __finally([&] {
            print(tuple(str("reraise finally")), str(" "), str("\n"));
        });
        try {
            return check_positive(n);
        } catch (ValueError &) {
            print(tuple(str("logging and passing it on")), str(" "), str("\n"));
            throw;
        }
    }
}

int64_t raise_bare_class(const str &RESTRICT key) {
    throw KeyError("");
}

int64_t siblings(const str &RESTRICT first, const str &RESTRICT second) {
    {
        bool __thrown = false;
        try {
            print(tuple(to_int(first)), str(" "), str("\n"));
        } catch (ValueError &) {
            __thrown = true;
            print(tuple(str("first was bad")), str(" "), str("\n"));
        }
        if (!__thrown) {
            print(tuple(str("first was fine")), str(" "), str("\n"));
        }
    }
    {
        bool __thrown = false;
        try {
            print(tuple(to_int(second)), str(" "), str("\n"));
        } catch (ValueError &) {
            __thrown = true;
            print(tuple(str("second was bad")), str(" "), str("\n"));
        }
        if (!__thrown) {
            print(tuple(str("second was fine")), str(" "), str("\n"));
        }
    }
    return 0LL;
}

int64_t relay() {
    try {
        throw ValueError(str("original"));
    } catch (PyException &e) {
        print(tuple(str("relaying")), str(" "), str("\n"));
        e.raise();
    }
}

int run() {
    list<int64_t> numbers;
    list<int64_t> items;
    int64_t i;
    print(tuple(guarded_parse(str("41"))), str(" "), str("\n"));
    try {
        print(tuple(guarded_parse(str("zzz"))), str(" "), str("\n"));
    } catch (ValueError &) {
        print(tuple(str("caught from callee")), str(" "), str("\n"));
    }
    print(tuple(parse_or(str("7"), 0LL)), str(" "), str("\n"));
    print(tuple(parse_or(str("seven"), 0LL)), str(" "), str("\n"));
    print(tuple(nested()), str(" "), str("\n"));
    print(tuple(handler_raises()), str(" "), str("\n"));
    print(tuple(check_positive(3LL)), str(" "), str("\n"));
    try {
        print(tuple(check_positive((-1LL))), str(" "), str("\n"));
    } catch (ValueError &) {
        print(tuple(str("caught the raise")), str(" "), str("\n"));
    }
    try {
        print(tuple(reraise((-2LL))), str(" "), str("\n"));
    } catch (ValueError &) {
        print(tuple(str("caught the re-raise")), str(" "), str("\n"));
    }
    try {
        print(tuple(raise_bare_class(str("k"))), str(" "), str("\n"));
    } catch (KeyError &) {
        print(tuple(str("caught the bare class")), str(" "), str("\n"));
    }
    print(tuple(siblings(str("8"), str("eight"))), str(" "), str("\n"));
    try {
        print(tuple(relay()), str(" "), str("\n"));
    } catch (ValueError &) {
        print(tuple(str("still a ValueError after the relay")), str(" "), str("\n"));
    }
    try {
        throw TypeError(str("wrong type"));
    } catch (PyException &) {
        print(tuple(str("base handler took the subclass")), str(" "), str("\n"));
    }
    numbers = list<int64_t>({1LL, 2LL, 3LL});
    try {
        print(tuple(numbers[10LL]), str(" "), str("\n"));
    } catch (IndexError &) {
        print(tuple(str("index error wins over the base class")), str(" "), str("\n"));
    } catch (PyException &) {
        print(tuple(str("not reached")), str(" "), str("\n"));
    }
    items = list<int64_t>({1LL, 2LL, 3LL});
    try {
        print(tuple(items[10LL]), str(" "), str("\n"));
    } catch (PyException &) {
        print(tuple(str("bare except caught it")), str(" "), str("\n"));
    }
    try {
        print(tuple(to_float(str("x"))), str(" "), str("\n"));
    } catch (ValueError &e) {
        print(tuple(str("float refused it")), str(" "), str("\n"));
    }
    int64_t __stop_3 = 3LL;
    for (i = 0; i < __stop_3; ++i) {
        {
            Finally __finally([&] {
                print(tuple(str("loop finally"), i), str(" "), str("\n"));
            });
            if (((i == 1LL))) {
                break;
            }
            print(tuple(str("loop"), i), str(" "), str("\n"));
        }
    }
    return 0LL;
}
}

namespace prog_files {

int run();

int run() {
    str SAMPLE;
    file handle;
    str text;
    file stepped;
    list<str> rest;
    file lines;
    str line;
    file out;
    str written;
    SAMPLE = str("tests/test_files/sample.txt");
    handle = open(SAMPLE);
    text = handle.read();
    print(tuple(len(text)), str(" "), str("\n"));
    print(tuple(text.splitlines()), str(" "), str("\n"));
    print(tuple(len(handle.read())), str(" "), str("\n"));
    handle.close();
    stepped = open(SAMPLE);
    print(tuple(stepped.readline().strip()), str(" "), str("\n"));
    print(tuple(stepped.readline().strip()), str(" "), str("\n"));
    rest = stepped.readlines();
    print(tuple(len(rest), rest[0LL].strip()), str(" "), str("\n"));
    print(tuple(len(stepped.readlines())), str(" "), str("\n"));
    lines = open(SAMPLE);
    auto && __range_15 = lines;
    for (auto &&__item_15 : __range_15) {
        line = __item_15;
        print(tuple(len(line), line.strip()), str(" "), str("\n"));
    }
    out = open(str("tests/test_files/sample_out.txt"), str("w"));
    print(tuple(out.write(str("alpha\n"))), str(" "), str("\n"));
    print(tuple(out.write(str("beta\n"))), str(" "), str("\n"));
    out.close();
    written = open(str("tests/test_files/sample_out.txt")).read();
    print(tuple(written.splitlines(), len(written)), str(" "), str("\n"));
    try {
        open(str("tests/test_files/no_such_file.txt"));
    } catch (FileNotFoundError &) {
        print(tuple(str("missing file raised")), str(" "), str("\n"));
    }
    return 0LL;
}
}

namespace prog_fixed_width_int_promotion {

uint16_t scale(const uint16_t &RESTRICT value, const uint16_t &RESTRICT factor);
int32_t clamp(const int32_t &RESTRICT value, const int32_t &RESTRICT low, const int32_t &RESTRICT high);
int run();

uint16_t scale(const uint16_t &RESTRICT value, const uint16_t &RESTRICT factor) {
    return (value * factor);
}

int32_t clamp(const int32_t &RESTRICT value, const int32_t &RESTRICT low, const int32_t &RESTRICT high) {
    if (((value < low))) {
        return low;
    }
    if (((value > high))) {
        return high;
    }
    return value;
}

int run() {
    uint16_t n;
    uint8_t m;
    uint16_t doubled;
    uint16_t total;
    n = 10LL;
    m = 200LL;
    print(tuple(n, m), str(" "), str("\n"));
    doubled = scale(n, 2LL);
    print(tuple(doubled), str(" "), str("\n"));
    total = (scale(n, 2LL) + n);
    print(tuple(total), str(" "), str("\n"));
    print(tuple(clamp(500LL, 0LL, 100LL)), str(" "), str("\n"));
    print(tuple(clamp((-5LL), 0LL, 100LL)), str(" "), str("\n"));
    print(tuple(clamp(50LL, 0LL, 100LL)), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_fixed_width_ints {

uint16_t add_u16(const uint16_t &RESTRICT a, const uint16_t &RESTRICT b);
int run();

uint16_t add_u16(const uint16_t &RESTRICT a, const uint16_t &RESTRICT b) {
    return uint16_t((a + b));
}

int run() {
    uint8_t a;
    int8_t b;
    uint16_t c;
    uint16_t d;
    int32_t e;
    uint32_t f;
    int64_t g;
    uint64_t h;
    list<uint16_t> values;
    dict<uint8_t, str> counts;
    set<int32_t> seen;
    a = uint8_t(200LL);
    b = int8_t((-100LL));
    print(tuple(a, b), str(" "), str("\n"));
    c = uint16_t(1000LL);
    d = add_u16(c, uint16_t(234LL));
    print(tuple(d), str(" "), str("\n"));
    e = int32_t((-70000LL));
    f = uint32_t(70000LL);
    print(tuple(e, f), str(" "), str("\n"));
    g = int64_t((-5000000000LL));
    h = uint64_t(5000000000LL);
    print(tuple(g, h), str(" "), str("\n"));
    print(tuple(((uint16_t(5LL) == uint16_t(5LL)))), str(" "), str("\n"));
    print(tuple(((uint16_t(5LL) < uint16_t(10LL)))), str(" "), str("\n"));
    values = list<uint16_t>({uint16_t(3LL), uint16_t(1LL), uint16_t(2LL)});
    print(tuple(values), str(" "), str("\n"));
    counts = dict<uint8_t, str>();
    counts[uint8_t(1LL)] = str("one");
    counts[uint8_t(2LL)] = str("two");
    print(tuple(counts[uint8_t(1LL)], counts[uint8_t(2LL)]), str(" "), str("\n"));
    seen = set<int32_t>({int32_t(1LL), int32_t(2LL), int32_t(2LL)});
    print(tuple(len(seen)), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_global_constexpr {
constexpr int64_t numCores = 21LL;
constexpr int64_t numInstruments = 3LL;
constexpr int64_t totalSlots = (numCores + numInstruments);
constexpr uint8_t spot = 0LL;
constexpr uint8_t future1 = 1LL;
constexpr uint8_t future2 = 2LL;
constexpr uint32_t pair0_mask = ((1LL << future1) | (1LL << spot));
constexpr uint32_t pair2_mask = ((1LL << future1) | (1LL << future2));

int run();

int run() {
    Array<uint8_t, 24> arr;
    print(tuple(str("Test 1 - derived global:"), totalSlots), str(" "), str("\n"));
    arr = Array<uint8_t, 24>();
    print(tuple(str("Test 2 - array sized by derived global:"), len(arr)), str(" "), str("\n"));
    print(tuple(str("Test 3 - shift masks:"), pair0_mask, pair2_mask), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_hotpath {

class Vector;

int64_t clamp(const int64_t &RESTRICT x, const int64_t &RESTRICT low, const int64_t &RESTRICT high);
int64_t normalize_price(const int64_t &RESTRICT raw, const int64_t &RESTRICT tick_size);
FLATTEN int64_t process_order(const int64_t &RESTRICT bid, const int64_t &RESTRICT ask, const int64_t &RESTRICT qty, const int64_t &RESTRICT tick_size);
int64_t square(const int64_t &RESTRICT x);
int64_t add(const int64_t &RESTRICT a, const int64_t &RESTRICT b);
FLATTEN int64_t distance_squared(const int64_t &RESTRICT x1, const int64_t &RESTRICT y1, const int64_t &RESTRICT x2, const int64_t &RESTRICT y2);
FLATTEN int64_t sum_up_to(const int64_t &RESTRICT n);
int run();

class Vector {
  public:
    int64_t x;
    int64_t y;

    Vector(const int64_t &RESTRICT x, const int64_t &RESTRICT y) { __init__(x, y); }

    Vector() = default;

    void __init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y);
    Vector scaled(const int64_t &RESTRICT factor);
    FLATTEN int64_t length_squared();
    FLATTEN void move(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy);
};

void Vector::__init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y) {
    this->x = x;
    this->y = y;
}

Vector Vector::scaled(const int64_t &RESTRICT factor) {
    return Vector((this->x * factor), (this->y * factor));
}

FLATTEN int64_t Vector::length_squared() {
    return distance_squared(0LL, 0LL, this->x, this->y);
}

FLATTEN void Vector::move(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy) {
    this->x = clamp((this->x + dx), (-100LL), 100LL);
    this->y = clamp((this->y + dy), (-100LL), 100LL);
}

int64_t clamp(const int64_t &RESTRICT x, const int64_t &RESTRICT low, const int64_t &RESTRICT high) {
    if (((x < low))) {
        return low;
    }
    if (((x > high))) {
        return high;
    }
    return x;
}

int64_t normalize_price(const int64_t &RESTRICT raw, const int64_t &RESTRICT tick_size) {
    int64_t rounded;
    rounded = (idiv(raw, tick_size) * tick_size);
    return clamp(rounded, 0LL, 1000000LL);
}

FLATTEN int64_t process_order(const int64_t &RESTRICT bid, const int64_t &RESTRICT ask, const int64_t &RESTRICT qty, const int64_t &RESTRICT tick_size) {
    int64_t norm_bid;
    int64_t norm_ask;
    int64_t spread;
    int64_t total;
    norm_bid = normalize_price(bid, tick_size);
    norm_ask = normalize_price(ask, tick_size);
    spread = (norm_ask - norm_bid);
    if (((spread < 0LL))) {
        spread = 0LL;
    }
    total = (spread * qty);
    if (((qty > 0LL))) {
        total = (total + qty);
    } else {
        total = (total - qty);
    }
    return total;
}

int64_t square(const int64_t &RESTRICT x) {
    return (x * x);
}

int64_t add(const int64_t &RESTRICT a, const int64_t &RESTRICT b) {
    return (a + b);
}

FLATTEN int64_t distance_squared(const int64_t &RESTRICT x1, const int64_t &RESTRICT y1, const int64_t &RESTRICT x2, const int64_t &RESTRICT y2) {
    int64_t dx;
    int64_t dy;
    dx = square((x2 - x1));
    dy = square((y2 - y1));
    return add(dx, dy);
}

FLATTEN int64_t sum_up_to(const int64_t &RESTRICT n) {
    int64_t total;
    int64_t i;
    total = 0LL;
    int64_t __stop_4 = n;
    for (i = 0; i < __stop_4; ++i) {
        total = add(total, i);
    }
    return total;
}

int run() {
    Vector v;
    Vector scaled;
    print(tuple(str("Test 1 - process_order:"), process_order(10007LL, 10023LL, 5LL, 5LL)), str(" "), str("\n"));
    print(tuple(str("Test 1b - process_order (sell):"), process_order(9998LL, 10001LL, (-3LL), 5LL)), str(" "), str("\n"));
    print(tuple(str("Test 2 - distance_squared:"), distance_squared(0LL, 0LL, 3LL, 4LL)), str(" "), str("\n"));
    print(tuple(str("Test 3 - sum_up_to:"), sum_up_to(5LL)), str(" "), str("\n"));
    v = Vector(3LL, 4LL);
    print(tuple(str("Test 4 - length_squared:"), v.length_squared()), str(" "), str("\n"));
    v.move(200LL, (-200LL));
    print(tuple(str("Test 5 - move (clamped):"), v.x, v.y), str(" "), str("\n"));
    scaled = v.scaled(2LL);
    print(tuple(str("Test 6 - scaled:"), scaled.x, scaled.y), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_iter {

int run();

int run() {
    list<int64_t> nums;
    list<int64_t> a;
    str s;
    list<int64_t> filtered;
    int64_t x;
    int64_t y;
    int64_t i;
    int64_t n;
    nums = list<int64_t>({1LL, 2LL, 3LL, 4LL, 5LL});
    a = list<int64_t>(map([](auto x) { return (x * 2LL); }, nums));
    print(tuple(a), str(" "), str("\n"));
    auto && __range_16 = map([](auto x) { return to_str(x); }, nums);
    for (auto &&__item_16 : __range_16) {
        s = __item_16;
        print(tuple(s), str(" "), str("\n"));
    }
    filtered = list<int64_t>(filter([](auto x) { return ((mod(x, 2LL) == 0LL)); }, nums));
    print(tuple(filtered), str(" "), str("\n"));
    auto && __range_17 = zip(nums, a);
    for (auto &&__item_17 : __range_17) {
        destructure(x, y) = __item_17;
        print(tuple(x, y), str(" "), str("\n"));
    }
    nums = copy(a);
    auto && __range_18 = enumerate(nums);
    for (auto &&__item_18 : __range_18) {
        destructure(i, n) = __item_18;
        print(tuple(i, n), str(" "), str("\n"));
    }
    return 0LL;
}
}

namespace prog_list {

list<int64_t> give_list(list<int64_t> &RESTRICT l);
int run();

list<int64_t> give_list(list<int64_t> &RESTRICT l) {
    print(tuple(l), str(" "), str("\n"));
    l.append(2LL);
    print(tuple(l), str(" "), str("\n"));
    return copy(l);
}

int run() {
    list<int64_t> l;
    int64_t a;
    list<int64_t> l2;
    int64_t x;
    int64_t y;
    int64_t z;
    list<int64_t> l3;
    int64_t n;
    list<int64_t> l4;
    list<int64_t> l5;
    print(tuple(list<int64_t>(list<int64_t>({1LL, 2LL, 3LL}))), str(" "), str("\n"));
    l = list<int64_t>({1LL, 2LL, 3LL});
    print(tuple(l), str(" "), str("\n"));
    l = list<int64_t>({1LL, 2LL, 3LL});
    print(tuple(l), str(" "), str("\n"));
    l.append(4LL);
    print(tuple(l), str(" "), str("\n"));
    l = give_list(l);
    print(tuple(l), str(" "), str("\n"));
    print(tuple(l[1LL]), str(" "), str("\n"));
    a = l[0LL];
    print(tuple(l), str(" "), str("\n"));
    l[0LL] = a;
    print(tuple(l), str(" "), str("\n"));
    l[0LL] = 2LL;
    print(tuple(l), str(" "), str("\n"));
    l2 = copy(l[slice(0LL, 1LL, std::nullopt)]);
    print(tuple(l2), str(" "), str("\n"));
    l.insert(0LL, 100LL);
    print(tuple(l), str(" "), str("\n"));
    l.insert(2LL, 200LL);
    print(tuple(l), str(" "), str("\n"));
    l.insert((-1LL), 300LL);
    print(tuple(l), str(" "), str("\n"));
    l.insert(100LL, 400LL);
    print(tuple(l), str(" "), str("\n"));
    l.insert((-100LL), 500LL);
    print(tuple(l), str(" "), str("\n"));
    l.remove(200LL);
    print(tuple(l), str(" "), str("\n"));
    x = l.pop();
    print(tuple(x, l), str(" "), str("\n"));
    y = l.pop(0LL);
    print(tuple(y, l), str(" "), str("\n"));
    z = l.pop((-2LL));
    print(tuple(z, l), str(" "), str("\n"));
    l.extend(list<int64_t>({7LL, 8LL}));
    print(tuple(l), str(" "), str("\n"));
    l3 = l.copy();
    print(tuple(l3), str(" "), str("\n"));
    l.clear();
    print(tuple(l), str(" "), str("\n"));
    l2 = list<int64_t>({5LL, 3LL, 1LL, 3LL, 9LL});
    print(tuple(l2.index(3LL)), str(" "), str("\n"));
    print(tuple(l2.index(3LL, 3LL)), str(" "), str("\n"));
    print(tuple(l2.index(3LL, 0LL, 2LL)), str(" "), str("\n"));
    print(tuple(l2.index(9LL, (-2LL))), str(" "), str("\n"));
    print(tuple(l2.count(3LL)), str(" "), str("\n"));
    print(tuple(l2.count(42LL)), str(" "), str("\n"));
    l2.sort();
    print(tuple(l2), str(" "), str("\n"));
    l2.sort(true);
    print(tuple(l2), str(" "), str("\n"));
    l2.sort(false);
    print(tuple(l2), str(" "), str("\n"));
    l2.reverse();
    print(tuple(l2), str(" "), str("\n"));
    n = len(l2);
    print(tuple(n), str(" "), str("\n"));
    print(tuple(l2[0LL], l2.back()), str(" "), str("\n"));
    if (!(((l2.back() == l2[(n - 1LL)])))) throw AssertionError("");
    l4 = list<int64_t>({1LL, 2LL, 3LL});
    print(tuple(l4.back()), str(" "), str("\n"));
    l4.back() += 10LL;
    if (!(((l4.back() == 13LL)))) throw AssertionError("");
    print(tuple(l4), str(" "), str("\n"));
    l5 = (list<int64_t>({1LL, 2LL}) * 3LL);
    print(tuple(l5), str(" "), str("\n"));
    l5 = (3LL * list<int64_t>({1LL, 2LL}));
    print(tuple(l5), str(" "), str("\n"));
    if (!(((l5 == (list<int64_t>({1LL, 2LL}) * 3LL))))) throw AssertionError("");
    l5 *= 2LL;
    print(tuple(l5), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_loops {

int run();

int run() {
    int64_t x;
    list<int64_t> l;
    int64_t i;
    int64_t step;
    int64_t n;
    x = 2LL;
    l = list<int64_t>({2LL, 3LL, 4LL});
    int64_t __len_1 = len(l);
    for (i = 0; i < __len_1; ++i) {
        print(tuple(l), str(" "), str("\n"));
    }
    int64_t __stop_5 = x;
    for (i = 0; i < __stop_5; ++i) {
        print(tuple(str("first"), i), str(" "), str("\n"));
    }
    int64_t __stop_6 = (x + 5LL);
    for (i = x; i < __stop_6; ++i) {
        print(tuple(str("second"), i), str(" "), str("\n"));
    }
    int64_t __stop_7 = (x + 10LL);
    for (i = x; i < __stop_7; i += 2) {
        print(tuple(str("third"), i), str(" "), str("\n"));
    }
    int64_t __stop_8 = (x - 7LL);
    for (i = x; i > __stop_8; i += -2) {
        print(tuple(str("fourth"), i), str(" "), str("\n"));
    }
    step = x;
    int64_t __stop_9 = (10LL * x);
    int64_t __step_0 = step;
    for (i = x;; i += __step_0) {
        if ((__step_0 > 0 && i >= __stop_9) || (__step_0 < 0 && i <= __stop_9)) break;
        print(tuple(str("fifth"), i), str(" "), str("\n"));
    }
    step = (-2LL);
    int64_t __stop_10 = (10LL * x);
    int64_t __step_1 = step;
    for (i = (5LL * x);; i += __step_1) {
        if ((__step_1 > 0 && i >= __stop_10) || (__step_1 < 0 && i <= __stop_10)) break;
        print(tuple(str("sixth"), i), str(" "), str("\n"));
    }
    auto && __range_19 = l;
    for (auto &&__item_19 : __range_19) {
        n = __item_19;
        print(tuple(str("seventh"), n), str(" "), str("\n"));
    }
    auto && __range_20 = l;
    for (auto &&__item_20 : __range_20) {
        n = __item_20;
        print(tuple(str("eight"), n), str(" "), str("\n"));
    }
    return 0LL;
}
}

namespace prog_math {

int run();

int run() {
    int64_t a;
    print(tuple(pow(10LL, 10LL)), str(" "), str("\n"));
    print(tuple(idiv((-10LL), 3LL)), str(" "), str("\n"));
    print(tuple(idiv(10LL, 3LL)), str(" "), str("\n"));
    print(tuple(fdiv(5LL, 2LL)), str(" "), str("\n"));
    print(tuple(pow(0.5, 4LL)), str(" "), str("\n"));
    print(tuple((50.0 * 100LL)), str(" "), str("\n"));
    a = pow(10LL, 10LL);
    print(tuple((-(-5LL))), str(" "), str("\n"));
    print(tuple((~5LL)), str(" "), str("\n"));
    print(tuple((-(+5LL))), str(" "), str("\n"));
    return a;
}
}

namespace prog_membership {

int run();

int run() {
    list<int64_t> l;
    dict<int64_t, int64_t> d;
    set<int64_t> s;
    str text;
    tuple<int64_t, int64_t, int64_t> t;
    list<tuple<int64_t, int64_t>> pairs;
    set<tuple<int64_t, int64_t>> pair_set;
    dict<tuple<int64_t, int64_t>, str> pair_dict;
    tuple<tuple<int64_t, int64_t>, tuple<int64_t, int64_t>> nested;
    list<tuple<str, int64_t>> mixed;
    list<str> strs;
    int64_t count;
    int64_t x;
    l = list<int64_t>({1LL, 2LL, 3LL});
    print(tuple((l.__contains__(2LL)), (l.__contains__(9LL))), str(" "), str("\n"));
    print(tuple((!l.__contains__(2LL)), (!l.__contains__(9LL))), str(" "), str("\n"));
    d = dict<int64_t, int64_t>({{1LL, 10LL}, {2LL, 20LL}});
    print(tuple((d.__contains__(1LL)), (d.__contains__(9LL))), str(" "), str("\n"));
    print(tuple((d.values().__contains__(10LL))), str(" "), str("\n"));
    print(tuple((!d.__contains__(1LL)), (!d.__contains__(9LL))), str(" "), str("\n"));
    s = set<int64_t>({1LL, 2LL, 3LL});
    print(tuple((s.__contains__(2LL)), (s.__contains__(9LL))), str(" "), str("\n"));
    print(tuple((!s.__contains__(2LL)), (!s.__contains__(9LL))), str(" "), str("\n"));
    text = str("hello world");
    print(tuple((text.__contains__(str("hello"))), (text.__contains__(str("zz")))), str(" "), str("\n"));
    print(tuple((text.__contains__(str("o w")))), str(" "), str("\n"));
    print(tuple((!text.__contains__(str("hello"))), (!text.__contains__(str("zz")))), str(" "), str("\n"));
    t = tuple(1LL, 2LL, 3LL);
    print(tuple((t.__contains__(2LL)), (t.__contains__(9LL))), str(" "), str("\n"));
    pairs = list<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});
    print(tuple((pairs.__contains__(tuple(1LL, 2LL))), (pairs.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));
    print(tuple((!pairs.__contains__(tuple(1LL, 2LL))), (!pairs.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));
    print(tuple((pairs.__contains__(tuple(2LL, 1LL)))), str(" "), str("\n"));
    pair_set = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});
    print(tuple((pair_set.__contains__(tuple(1LL, 2LL))), (pair_set.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));
    print(tuple((pair_set.__contains__(tuple(2LL, 1LL)))), str(" "), str("\n"));
    pair_dict = dict<tuple<int64_t, int64_t>, str>({{tuple(1LL, 2LL), str("a")}, {tuple(3LL, 4LL), str("b")}});
    print(tuple((pair_dict.__contains__(tuple(1LL, 2LL))), (pair_dict.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));
    nested = tuple(tuple(1LL, 2LL), tuple(3LL, 4LL));
    print(tuple((nested.__contains__(tuple(1LL, 2LL))), (nested.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));
    mixed = list<tuple<str, int64_t>>({tuple(str("a"), 1LL), tuple(str("b"), 2LL)});
    print(tuple((mixed.__contains__(tuple(str("a"), 1LL))), (mixed.__contains__(tuple(str("a"), 2LL)))), str(" "), str("\n"));
    strs = list<str>({str("a"), str("b")});
    print(tuple((strs.__contains__(str("a"))), (strs.__contains__(str("z")))), str(" "), str("\n"));
    if ((l.__contains__(2LL))) {
        print(tuple(str("found")), str(" "), str("\n"));
    } else {
        print(tuple(str("missing")), str(" "), str("\n"));
    }
    count = 0LL;
    auto && __range_21 = list<int64_t>({1LL, 2LL, 3LL, 4LL});
    for (auto &&__item_21 : __range_21) {
        x = __item_21;
        if ((s.__contains__(x))) {
            count = (count + 1LL);
        }
    }
    print(tuple(count), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_print {

int run();

int run() {
    int64_t a;
    int64_t b;
    int64_t c;
    a = 1LL;
    b = 2LL;
    c = 3LL;
    print(tuple(), str(" "), str("\n"));
    print(tuple(a), str(" "), str("\n"));
    print(tuple(a, b, c), str(" "), str("\n"));
    print(tuple(), str(" "), str("end"));
    print(tuple(), str("sep"), str("\n"));
    print(tuple(a, b, c), str("-"), str("\n"));
    print(tuple(a), str(" "), str("()"));
    print(tuple(b), str(" "), str("()"));
    print(tuple(c), str(" "), str("\n"));
    print(tuple(a, b, c), str("sep"), str("end"));
    return 0LL;
}
}

namespace prog_set {

int run();

int run() {
    set<int64_t> s;
    set<int64_t> a;
    set<int64_t> b;
    set<int64_t> small;
    set<int64_t> big;
    set<int64_t> nine;
    set<int64_t> c;
    set<int64_t> d;
    set<int64_t> keep;
    set<int64_t> drop;
    int64_t total;
    int64_t x;
    set<tuple<int64_t, int64_t>> s1;
    set<tuple<int64_t, int64_t>> s2;
    set<str> s3;
    s = set<int64_t>({3LL, 1LL, 2LL, 1LL});
    print(tuple(len(s)), str(" "), str("\n"));
    print(tuple(sorted(s, false)), str(" "), str("\n"));
    print(tuple(sorted(s, true)), str(" "), str("\n"));
    s.add(4LL);
    print(tuple(sorted(s, false)), str(" "), str("\n"));
    s.add(4LL);
    print(tuple(len(s)), str(" "), str("\n"));
    s.remove(4LL);
    print(tuple(sorted(s, false)), str(" "), str("\n"));
    s.discard(99LL);
    print(tuple(sorted(s, false)), str(" "), str("\n"));
    a = set<int64_t>({1LL, 2LL, 3LL});
    b = set<int64_t>({3LL, 4LL});
    print(tuple(sorted((a | b), false)), str(" "), str("\n"));
    print(tuple(sorted((a & b), false)), str(" "), str("\n"));
    print(tuple(sorted((a - b), false)), str(" "), str("\n"));
    print(tuple(sorted((a ^ b), false)), str(" "), str("\n"));
    print(tuple(sorted(a.union_(b), false)), str(" "), str("\n"));
    print(tuple(sorted(a.intersection(b), false)), str(" "), str("\n"));
    print(tuple(sorted(a.difference(b), false)), str(" "), str("\n"));
    print(tuple(sorted(a.symmetric_difference(b), false)), str(" "), str("\n"));
    print(tuple(sorted(a, false), sorted(b, false)), str(" "), str("\n"));
    small = set<int64_t>({1LL, 2LL});
    big = set<int64_t>({1LL, 2LL, 3LL});
    print(tuple(small.issubset(big)), str(" "), str("\n"));
    print(tuple(big.issuperset(small)), str(" "), str("\n"));
    nine = set<int64_t>({9LL});
    print(tuple(small.isdisjoint(nine)), str(" "), str("\n"));
    print(tuple(small.isdisjoint(big)), str(" "), str("\n"));
    print(tuple(((small <= big)), ((small < big))), str(" "), str("\n"));
    print(tuple(((big >= small)), ((big > small))), str(" "), str("\n"));
    print(tuple(((set<int64_t>({1LL, 2LL}) == set<int64_t>({2LL, 1LL})))), str(" "), str("\n"));
    print(tuple(((small != big))), str(" "), str("\n"));
    c = a.copy();
    c.add(99LL);
    print(tuple(sorted(a, false), sorted(c, false)), str(" "), str("\n"));
    d = set<int64_t>({1LL, 2LL, 3LL});
    d.update(set<int64_t>({4LL}));
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    keep = set<int64_t>({2LL, 3LL, 4LL});
    d.intersection_update(keep);
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    drop = set<int64_t>({4LL});
    d.difference_update(drop);
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    d.symmetric_difference_update(set<int64_t>({3LL, 5LL}));
    print(tuple(sorted(d, false)), str(" "), str("\n"));
    d.clear();
    print(tuple(len(d)), str(" "), str("\n"));
    total = 0LL;
    auto && __range_22 = set<int64_t>({1LL, 2LL, 3LL});
    for (auto &&__item_22 : __range_22) {
        x = __item_22;
        total = (total + x);
    }
    print(tuple(total), str(" "), str("\n"));
    s1 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 3LL)});
    print(tuple(sorted(s1, false)), str(" "), str("\n"));
    s2 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 2LL)});
    print(tuple(sorted(s2, false)), str(" "), str("\n"));
    s3 = set<str>({str("Hello"), str("World")});
    print(tuple(sorted(s3, false)), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_slice {

int run();

int run() {
    list<int64_t> l;
    list<int64_t> empty;
    list<int64_t> original;
    list<int64_t> copied;
    str s;
    l = list<int64_t>({0LL, 1LL, 2LL, 3LL, 4LL, 5LL});
    print(tuple(l[slice(0LL, 1LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(1LL, 4LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, 3LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(3LL, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, std::nullopt, 2LL)]), str(" "), str("\n"));
    print(tuple(l[slice(1LL, 5LL, 2LL)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, std::nullopt, 3LL)]), str(" "), str("\n"));
    print(tuple(l[slice((-3LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, (-2LL), std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice((-4LL), (-1LL), std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice((-1LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));
    print(tuple(l[slice(4LL, 1LL, (-1LL))]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, std::nullopt, (-2LL))]), str(" "), str("\n"));
    print(tuple(l[slice((-1LL), (-4LL), (-1LL))]), str(" "), str("\n"));
    print(tuple(l[slice(10LL, 20LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice((-100LL), 100LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(std::nullopt, 100LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice((-100LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(2LL, 2LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(4LL, 1LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(l[slice(1LL, 4LL, (-1LL))]), str(" "), str("\n"));
    empty = list<int64_t>();
    print(tuple(empty[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(empty[slice(0LL, 5LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(empty[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));
    original = list<int64_t>({1LL, 2LL, 3LL});
    copied = copy(original[slice(std::nullopt, std::nullopt, std::nullopt)]);
    copied.append(4LL);
    print(tuple(original, copied), str(" "), str("\n"));
    s = str("abcdef");
    print(tuple(s[slice(0LL, 1LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(1LL, 4LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, 3LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(3LL, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, std::nullopt, 2LL)]), str(" "), str("\n"));
    print(tuple(s[slice(1LL, 5LL, 2LL)]), str(" "), str("\n"));
    print(tuple(s[slice((-3LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, (-2LL), std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice((-4LL), (-1LL), std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));
    print(tuple(s[slice(4LL, 1LL, (-1LL))]), str(" "), str("\n"));
    print(tuple(s[slice(std::nullopt, std::nullopt, (-2LL))]), str(" "), str("\n"));
    print(tuple(s[slice(100LL, 200LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice((-100LL), 100LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(2LL, 2LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(s[slice(4LL, 1LL, std::nullopt)]), str(" "), str("\n"));
    print(tuple(str("")[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));
    print(tuple(str("")[slice(0LL, 5LL, std::nullopt)]), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_string {

int run();

int run() {
    str s;
    str padded;
    str a;
    str b;
    str joined;
    str c;
    s = str("Hello World");
    print(tuple(s), str(" "), str("\n"));
    print(tuple(len(s)), str(" "), str("\n"));
    print(tuple(s[0LL], s.back()), str(" "), str("\n"));
    print(tuple(s.upper()), str(" "), str("\n"));
    print(tuple(s.lower()), str(" "), str("\n"));
    print(tuple(s.swapcase()), str(" "), str("\n"));
    print(tuple(s.capitalize()), str(" "), str("\n"));
    print(tuple(str("hello world").title()), str(" "), str("\n"));
    print(tuple(s.casefold()), str(" "), str("\n"));
    print(tuple(s.find(str("o"))), str(" "), str("\n"));
    print(tuple(s.find(str("o"), 5LL)), str(" "), str("\n"));
    print(tuple(s.rfind(str("o"))), str(" "), str("\n"));
    print(tuple(s.find(str("zz"))), str(" "), str("\n"));
    print(tuple(s.index(str("World"))), str(" "), str("\n"));
    print(tuple(s.count(str("l"))), str(" "), str("\n"));
    print(tuple(s.count(str("zz"))), str(" "), str("\n"));
    print(tuple(s.startswith(str("Hello"))), str(" "), str("\n"));
    print(tuple(s.startswith(str("World"))), str(" "), str("\n"));
    print(tuple(s.endswith(str("World"))), str(" "), str("\n"));
    print(tuple(s.replace(str("l"), str("L"))), str(" "), str("\n"));
    print(tuple(s.replace(str("l"), str("L"), 2LL)), str(" "), str("\n"));
    print(tuple(s.removeprefix(str("Hello "))), str(" "), str("\n"));
    print(tuple(s.removesuffix(str(" World"))), str(" "), str("\n"));
    padded = str("  spaced  ");
    print(tuple(padded.strip()), str(" "), str("\n"));
    print(tuple(padded.lstrip()), str(" "), str("\n"));
    print(tuple(padded.rstrip()), str(" "), str("\n"));
    print(tuple(str("xxhixx").strip(str("x"))), str(" "), str("\n"));
    print(tuple(str("hi").ljust(5LL, str("."))), str(" "), str("\n"));
    print(tuple(str("hi").rjust(5LL, str("."))), str(" "), str("\n"));
    print(tuple(str("hi").center(6LL, str("."))), str(" "), str("\n"));
    print(tuple(str("42").zfill(5LL)), str(" "), str("\n"));
    print(tuple(str("-42").zfill(5LL)), str(" "), str("\n"));
    print(tuple(str("abc").isalpha(), str("a1").isalpha()), str(" "), str("\n"));
    print(tuple(str("123").isdigit(), str("12a").isdigit()), str(" "), str("\n"));
    print(tuple(str("a1").isalnum(), str("a-1").isalnum()), str(" "), str("\n"));
    print(tuple(str("  ").isspace(), str("a ").isspace()), str(" "), str("\n"));
    print(tuple(str("ABC").isupper(), str("Abc").isupper()), str(" "), str("\n"));
    print(tuple(str("abc").islower(), str("Abc").islower()), str(" "), str("\n"));
    print(tuple(s.split()), str(" "), str("\n"));
    print(tuple(str("a,b,c").split(str(","))), str(" "), str("\n"));
    print(tuple(str("a,,b").split(str(","))), str(" "), str("\n"));
    print(tuple(str("-").join(str("a,b,c").split(str(",")))), str(" "), str("\n"));
    print(tuple(str("one\ntwo").splitlines()), str(" "), str("\n"));
    a = str("foo");
    b = str("bar");
    print(tuple((a + b)), str(" "), str("\n"));
    print(tuple((a * 3LL)), str(" "), str("\n"));
    print(tuple(((a == str("foo"))), ((a == b))), str(" "), str("\n"));
    print(tuple(((a < b)), ((a > b))), str(" "), str("\n"));
    print(tuple(to_str(42LL)), str(" "), str("\n"));
    print(tuple(to_str(3.5)), str(" "), str("\n"));
    print(tuple(to_str(true)), str(" "), str("\n"));
    print(tuple(to_int(str("100"))), str(" "), str("\n"));
    print(tuple(to_float(str("0.5"))), str(" "), str("\n"));
    joined = str("");
    auto && __range_23 = str("abc");
    for (auto &&__item_23 : __range_23) {
        c = __item_23;
        joined = ((joined + c) + str("."));
    }
    print(tuple(joined), str(" "), str("\n"));
    return 0LL;
}
}

namespace prog_truthy {

int run();

int run() {
    int64_t a;
    int64_t b;
    str s1;
    str s2;
    list<int64_t> empty;
    list<int64_t> full;
    int64_t n;
    a = 0LL;
    b = 5LL;
    if (a) {
        print(tuple(str("a truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("a falsy")), str(" "), str("\n"));
    }
    if (b) {
        print(tuple(str("b truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("b falsy")), str(" "), str("\n"));
    }
    s1 = str("");
    s2 = str("hello");
    if (to_bool(s1)) {
        print(tuple(str("s1 truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("s1 falsy")), str(" "), str("\n"));
    }
    if (to_bool(s2)) {
        print(tuple(str("s2 truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("s2 falsy")), str(" "), str("\n"));
    }
    empty = list<int64_t>();
    full = list<int64_t>({1LL, 2LL, 3LL});
    if (to_bool(empty)) {
        print(tuple(str("empty truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("empty falsy")), str(" "), str("\n"));
    }
    if (to_bool(full)) {
        print(tuple(str("full truthy")), str(" "), str("\n"));
    } else {
        print(tuple(str("full falsy")), str(" "), str("\n"));
    }
    print(tuple((!to_bool(a))), str(" "), str("\n"));
    print(tuple((!to_bool(b))), str(" "), str("\n"));
    print(tuple(to_bool(a)), str(" "), str("\n"));
    print(tuple(to_bool(b)), str(" "), str("\n"));
    print(tuple(to_bool(0.0)), str(" "), str("\n"));
    print(tuple(to_bool(1.5)), str(" "), str("\n"));
    print(tuple(to_bool(true)), str(" "), str("\n"));
    print(tuple(to_bool(false)), str(" "), str("\n"));
    n = 3LL;
    while (n) {
        print(tuple(n), str(" "), str("\n"));
        n = (n - 1LL);
    }
    return 0LL;
}
}

namespace prog_tuple {

int run();

int run() {
    int64_t a;
    int64_t b;
    int64_t x;
    str s;
    int64_t p;
    int64_t q;
    int64_t m;
    int64_t n;
    tuple<int64_t, int64_t> t;
    tuple<int64_t, int64_t> t2;
    int64_t z;
    destructure(a, b) = tuple(1LL, 2LL);
    print(tuple(str("Test 1 - Simple destructure:"), a, b), str(" "), str("\n"));
    destructure(a, b) = tuple(10LL, 20LL);
    print(tuple(str("Test 2 - Reassign:"), a, b), str(" "), str("\n"));
    destructure(x, s) = tuple(42LL, str("hello"));
    print(tuple(str("Test 3 - Mixed types:"), x, s), str(" "), str("\n"));
    destructure(p, q) = tuple(100LL, 200LL);
    destructure(m, n) = tuple(p, q);
    print(tuple(str("Test 4 - Chained destructure:"), m, n), str(" "), str("\n"));
    t = tuple(1LL, 2LL);
    t2 = copy(t);
    z = t.get<0>();
    z = t.get<1>();
    return 0LL;
}
}

#include <cstring>

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "array.py") == 0) return prog_array::run();
    if (argc > 1 && std::strcmp(argv[1], "boolops.py") == 0) return prog_boolops::run();
    if (argc > 1 && std::strcmp(argv[1], "builtin_functions.py") == 0) return prog_builtin_functions::run();
    if (argc > 1 && std::strcmp(argv[1], "bytes.py") == 0) return prog_bytes::run();
    if (argc > 1 && std::strcmp(argv[1], "casts.py") == 0) return prog_casts::run();
    if (argc > 1 && std::strcmp(argv[1], "classes.py") == 0) return prog_classes::run();
    if (argc > 1 && std::strcmp(argv[1], "comparison.py") == 0) return prog_comparison::run();
    if (argc > 1 && std::strcmp(argv[1], "comprehensions.py") == 0) return prog_comprehensions::run();
    if (argc > 1 && std::strcmp(argv[1], "dict.py") == 0) return prog_dict::run();
    if (argc > 1 && std::strcmp(argv[1], "exceptions.py") == 0) return prog_exceptions::run();
    if (argc > 1 && std::strcmp(argv[1], "files.py") == 0) return prog_files::run();
    if (argc > 1 && std::strcmp(argv[1], "fixed_width_int_promotion.py") == 0) return prog_fixed_width_int_promotion::run();
    if (argc > 1 && std::strcmp(argv[1], "fixed_width_ints.py") == 0) return prog_fixed_width_ints::run();
    if (argc > 1 && std::strcmp(argv[1], "global_constexpr.py") == 0) return prog_global_constexpr::run();
    if (argc > 1 && std::strcmp(argv[1], "hotpath.py") == 0) return prog_hotpath::run();
    if (argc > 1 && std::strcmp(argv[1], "iter.py") == 0) return prog_iter::run();
    if (argc > 1 && std::strcmp(argv[1], "list.py") == 0) return prog_list::run();
    if (argc > 1 && std::strcmp(argv[1], "loops.py") == 0) return prog_loops::run();
    if (argc > 1 && std::strcmp(argv[1], "math.py") == 0) return prog_math::run();
    if (argc > 1 && std::strcmp(argv[1], "membership.py") == 0) return prog_membership::run();
    if (argc > 1 && std::strcmp(argv[1], "print.py") == 0) return prog_print::run();
    if (argc > 1 && std::strcmp(argv[1], "set.py") == 0) return prog_set::run();
    if (argc > 1 && std::strcmp(argv[1], "slice.py") == 0) return prog_slice::run();
    if (argc > 1 && std::strcmp(argv[1], "string.py") == 0) return prog_string::run();
    if (argc > 1 && std::strcmp(argv[1], "truthy.py") == 0) return prog_truthy::run();
    if (argc > 1 && std::strcmp(argv[1], "tuple.py") == 0) return prog_tuple::run();
    return 1;
}

