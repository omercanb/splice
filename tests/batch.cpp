#include "runtime.h"
using namespace py;

namespace prog_array {
constexpr int64_t numInstruments = 3LL;

int run();    // 8

int run() {    // 8
    Array<uint32_t, 3> a;    // 10
    Array<uint32_t, 4> b;    // 15
    int64_t total;    // 29
    uint32_t v;    // 30
    a = Array<uint32_t, 3>();    // 10
    a[1LL] = 5LL;    // 11
    print(tuple(str("Test 1 - index/assign:"), a[0LL], a[1LL], a[2LL]), str(" "), str("\n"));    // 12
    b = Array<uint32_t, 4>();    // 15
    print(tuple(str("Test 2 - Literal size:"), b[0LL], b[3LL]), str(" "), str("\n"));    // 16
    print(tuple(str("Test 3 - last element:"), a.back()), str(" "), str("\n"));    // 19
    print(tuple(str("Test 4 - len:"), len(a), len(b)), str(" "), str("\n"));    // 22
    b.fill(uint32_t(1LL));    // 25
    print(tuple(str("Test 5 - fill:"), b[0LL], b[1LL], b[2LL], b[3LL]), str(" "), str("\n"));    // 26
    total = 0LL;    // 29
    auto && __range_0 = b;    // 30
    for (auto &&__item_0 : __range_0) {    // 30
        v = __item_0;    // 30
        total = (total + v);    // 31
    }    // 30
    print(tuple(str("Test 6 - iteration sum:"), total), str(" "), str("\n"));    // 32
    print(tuple(str("Test 7 - str:"), a), str(" "), str("\n"));    // 35
    return 0LL;    // 37
}    // 8
}

namespace prog_boolops {

int64_t side(const int64_t &RESTRICT v);    // 1
int run();    // 6

int64_t side(const int64_t &RESTRICT v) {    // 1
    print(tuple(str("SIDE")), str(" "), str("\n"));    // 2
    return v;    // 3
}    // 1

int run() {    // 6
    int64_t a;    // 7
    int64_t b;    // 8
    int64_t zero;    // 9
    str empty;    // 19
    str text;    // 20
    list<int64_t> no_items;    // 26
    double f;    // 32
    double g;    // 33
    int64_t n;    // 64
    a = 1LL;    // 7
    b = 2LL;    // 8
    zero = 0LL;    // 9
    print(tuple(_and(a, b)), str(" "), str("\n"));    // 12
    print(tuple(_or(a, b)), str(" "), str("\n"));    // 13
    print(tuple(_and(zero, b)), str(" "), str("\n"));    // 14
    print(tuple(_or(zero, b)), str(" "), str("\n"));    // 15
    print(tuple(_and(zero, zero)), str(" "), str("\n"));    // 16
    print(tuple(_or(zero, zero)), str(" "), str("\n"));    // 17
    empty = str("");    // 19
    text = str("hi");    // 20
    print(tuple(_or(empty, text)), str(" "), str("\n"));    // 21
    print(tuple(_and(text, empty)), str(" "), str("\n"));    // 22
    print(tuple(_or(text, empty)), str(" "), str("\n"));    // 23
    print(tuple(_and(empty, text)), str(" "), str("\n"));    // 24
    no_items = list<int64_t>();    // 26
    print(tuple(_or(no_items, list<int64_t>({1LL, 2LL}))), str(" "), str("\n"));    // 27
    print(tuple(_and(list<int64_t>({3LL}), no_items)), str(" "), str("\n"));    // 28
    print(tuple(_or(list<int64_t>({3LL}), list<int64_t>({4LL}))), str(" "), str("\n"));    // 29
    print(tuple(_and(no_items, list<int64_t>({4LL}))), str(" "), str("\n"));    // 30
    f = 0.0;    // 32
    g = 2.5;    // 33
    print(tuple(_or(f, g)), str(" "), str("\n"));    // 34
    print(tuple(_and(g, f)), str(" "), str("\n"));    // 35
    print(tuple(_and(zero, side(9LL))), str(" "), str("\n"));    // 38
    print(tuple(_or(a, side(9LL))), str(" "), str("\n"));    // 39
    print(tuple(_and(a, side(9LL))), str(" "), str("\n"));    // 40
    print(tuple(_or(zero, side(9LL))), str(" "), str("\n"));    // 41
    print(tuple(_and(a, _and(b, 3LL))), str(" "), str("\n"));    // 44
    print(tuple(_or(zero, _or(zero, 3LL))), str(" "), str("\n"));    // 45
    print(tuple(_or(_and(a, b), 3LL)), str(" "), str("\n"));    // 46
    print(tuple(_or(zero, _and(b, 3LL))), str(" "), str("\n"));    // 47
    print(tuple((_and(a, b) + 1LL)), str(" "), str("\n"));    // 48
    if ((a && to_bool(text))) {    // 51
        print(tuple(str("cond and")), str(" "), str("\n"));    // 52
    }    // 51
    if ((to_bool(empty) || a)) {    // 53
        print(tuple(str("cond or")), str(" "), str("\n"));    // 54
    }    // 53
    if ((!(to_bool(empty) || zero))) {    // 55
        print(tuple(str("cond not")), str(" "), str("\n"));    // 56
    }    // 55
    if ((a && (to_bool(text) && b))) {    // 57
        print(tuple(str("cond chained")), str(" "), str("\n"));    // 58
    }    // 57
    if ((to_bool(no_items) || to_bool(text))) {    // 59
        print(tuple(str("cond mixed")), str(" "), str("\n"));    // 60
    }    // 59
    if ((zero && side(9LL))) {    // 61
        print(tuple(str("unreachable")), str(" "), str("\n"));    // 62
    }    // 61
    n = 0LL;    // 64
    while ((((n < 2LL)) && to_bool(text))) {    // 65
        n = (n + 1LL);    // 66
    }    // 65
    print(tuple(n), str(" "), str("\n"));    // 67
    print(tuple((!to_bool(a))), str(" "), str("\n"));    // 70
    print(tuple((!to_bool(zero))), str(" "), str("\n"));    // 71
    print(tuple((!to_bool(empty))), str(" "), str("\n"));    // 72
    print(tuple((!to_bool(_and(a, b)))), str(" "), str("\n"));    // 73
    return 0LL;    // 75
}    // 6
}

namespace prog_builtin_functions {

list<int64_t> __list_comprehension_0(const list<int64_t> &RESTRICT numbers);    // 31
int run();    // 1

list<int64_t> __list_comprehension_0(const list<int64_t> &RESTRICT numbers) {    // 31
    list<int64_t> __tmp_0;    // 31
    int64_t v;    // 31
    __tmp_0 = list<int64_t>();    // 31
    auto && __range_1 = numbers;    // 31
    for (auto &&__item_1 : __range_1) {    // 31
        v = __item_1;    // 31
        __tmp_0.append((v * 2LL));    // 31
    }    // 31
    return __tmp_0;    // 31
}    // 31

int run() {    // 1
    list<int64_t> numbers;    // 2
    list<int64_t> empty;    // 3
    list<int64_t> zeros;    // 10
    list<int64_t> with_zero;    // 11
    numbers = list<int64_t>({4LL, 1LL, 7LL, 3LL});    // 2
    empty = list<int64_t>();    // 3
    print(tuple(sum(numbers), sum(empty)), str(" "), str("\n"));    // 5
    print(tuple(min(numbers), max(numbers)), str(" "), str("\n"));    // 6
    print(tuple(min(3LL, 8LL), max(3LL, 8LL), min(2.5, 1.5)), str(" "), str("\n"));    // 7
    zeros = list<int64_t>({0LL, 0LL});    // 10
    with_zero = list<int64_t>({1LL, 0LL, 2LL});    // 11
    print(tuple(any(numbers), any(zeros), any(empty)), str(" "), str("\n"));    // 12
    print(tuple(all(numbers), all(with_zero), all(empty)), str(" "), str("\n"));    // 13
    print(tuple(sum(range(5LL)), max(range(5LL))), str(" "), str("\n"));    // 16
    print(tuple(sum(set<int64_t>({1LL, 2LL, 3LL}))), str(" "), str("\n"));    // 17
    print(tuple(min(str("hello")), max(str("hello"))), str(" "), str("\n"));    // 18
    print(tuple(divmod(7LL, 2LL), divmod((-7LL), 2LL)), str(" "), str("\n"));    // 20
    print(tuple(py::round(2.5), py::round(3.5), py::round((-2.5))), str(" "), str("\n"));    // 23
    print(tuple(py::round(2.567, 1LL), py::round(2.567, 2LL)), str(" "), str("\n"));    // 24
    print(tuple(py::round(5LL)), str(" "), str("\n"));    // 25
    print(tuple(chr(65LL), 65LL), str(" "), str("\n"));    // 27
    print(tuple(chr((97LL + 1LL))), str(" "), str("\n"));    // 28
    print(tuple(sum(__list_comprehension_0(numbers))), str(" "), str("\n"));    // 31
    return 0LL;    // 33
}    // 1
}

namespace prog_bytes {

int run();    // 1

int run() {    // 1
    bytes b;    // 2
    bytes padded;    // 29
    bytes a;    // 54
    bytes c;    // 55
    int64_t total;    // 68
    int64_t byte;    // 69
    bytes joined;    // 73
    b = bytes(std::string("Hello World", 11));    // 2
    print(tuple(b), str(" "), str("\n"));    // 3
    print(tuple(len(b)), str(" "), str("\n"));    // 4
    print(tuple(b[0LL], b.back()), str(" "), str("\n"));    // 5
    print(tuple(b.upper()), str(" "), str("\n"));    // 7
    print(tuple(b.lower()), str(" "), str("\n"));    // 8
    print(tuple(b.swapcase()), str(" "), str("\n"));    // 9
    print(tuple(b.capitalize()), str(" "), str("\n"));    // 10
    print(tuple(bytes(std::string("hello world", 11)).title()), str(" "), str("\n"));    // 11
    print(tuple(b.find(bytes(std::string("o", 1)))), str(" "), str("\n"));    // 13
    print(tuple(b.find(bytes(std::string("o", 1)), 5LL)), str(" "), str("\n"));    // 14
    print(tuple(b.rfind(bytes(std::string("o", 1)))), str(" "), str("\n"));    // 15
    print(tuple(b.find(bytes(std::string("zz", 2)))), str(" "), str("\n"));    // 16
    print(tuple(b.index(bytes(std::string("World", 5)))), str(" "), str("\n"));    // 17
    print(tuple(b.count(bytes(std::string("l", 1)))), str(" "), str("\n"));    // 18
    print(tuple(b.count(bytes(std::string("zz", 2)))), str(" "), str("\n"));    // 19
    print(tuple(b.startswith(bytes(std::string("Hello", 5)))), str(" "), str("\n"));    // 20
    print(tuple(b.startswith(bytes(std::string("World", 5)))), str(" "), str("\n"));    // 21
    print(tuple(b.endswith(bytes(std::string("World", 5)))), str(" "), str("\n"));    // 22
    print(tuple(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1)))), str(" "), str("\n"));    // 24
    print(tuple(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1)), 2LL)), str(" "), str("\n"));    // 25
    print(tuple(b.removeprefix(bytes(std::string("Hello ", 6)))), str(" "), str("\n"));    // 26
    print(tuple(b.removesuffix(bytes(std::string(" World", 6)))), str(" "), str("\n"));    // 27
    padded = bytes(std::string("  spaced  ", 10));    // 29
    print(tuple(padded.strip()), str(" "), str("\n"));    // 30
    print(tuple(padded.lstrip()), str(" "), str("\n"));    // 31
    print(tuple(padded.rstrip()), str(" "), str("\n"));    // 32
    print(tuple(bytes(std::string("xxhixx", 6)).strip(bytes(std::string("x", 1)))), str(" "), str("\n"));    // 33
    print(tuple(bytes(std::string("hi", 2)).ljust(5LL, bytes(std::string(".", 1)))), str(" "), str("\n"));    // 35
    print(tuple(bytes(std::string("hi", 2)).rjust(5LL, bytes(std::string(".", 1)))), str(" "), str("\n"));    // 36
    print(tuple(bytes(std::string("hi", 2)).center(6LL, bytes(std::string(".", 1)))), str(" "), str("\n"));    // 37
    print(tuple(bytes(std::string("42", 2)).zfill(5LL)), str(" "), str("\n"));    // 38
    print(tuple(bytes(std::string("-42", 3)).zfill(5LL)), str(" "), str("\n"));    // 39
    print(tuple(bytes(std::string("abc", 3)).isalpha(), bytes(std::string("a1", 2)).isalpha()), str(" "), str("\n"));    // 41
    print(tuple(bytes(std::string("123", 3)).isdigit(), bytes(std::string("12a", 3)).isdigit()), str(" "), str("\n"));    // 42
    print(tuple(bytes(std::string("a1", 2)).isalnum(), bytes(std::string("a-1", 3)).isalnum()), str(" "), str("\n"));    // 43
    print(tuple(bytes(std::string("  ", 2)).isspace(), bytes(std::string("a ", 2)).isspace()), str(" "), str("\n"));    // 44
    print(tuple(bytes(std::string("ABC", 3)).isupper(), bytes(std::string("Abc", 3)).isupper()), str(" "), str("\n"));    // 45
    print(tuple(bytes(std::string("abc", 3)).islower(), bytes(std::string("Abc", 3)).islower()), str(" "), str("\n"));    // 46
    print(tuple(b.split()), str(" "), str("\n"));    // 48
    print(tuple(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1)))), str(" "), str("\n"));    // 49
    print(tuple(bytes(std::string("a,,b", 4)).split(bytes(std::string(",", 1)))), str(" "), str("\n"));    // 50
    print(tuple(bytes(std::string("-", 1)).join(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1))))), str(" "), str("\n"));    // 51
    print(tuple(bytes(std::string("one\ntwo", 7)).splitlines()), str(" "), str("\n"));    // 52
    a = bytes(std::string("foo", 3));    // 54
    c = bytes(std::string("bar", 3));    // 55
    print(tuple((a + c)), str(" "), str("\n"));    // 56
    print(tuple((a * 3LL)), str(" "), str("\n"));    // 57
    print(tuple(((a == bytes(std::string("foo", 3)))), ((a == c))), str(" "), str("\n"));    // 58
    print(tuple(((a < c)), ((a > c))), str(" "), str("\n"));    // 59
    print(tuple((b.__contains__(bytes(std::string("World", 5))))), str(" "), str("\n"));    // 61
    print(tuple((b.__contains__(87LL))), str(" "), str("\n"));    // 62
    print(tuple((b.__contains__(90LL))), str(" "), str("\n"));    // 63
    print(tuple(bytes(3LL)), str(" "), str("\n"));    // 65
    print(tuple(bytes(list<int64_t>({65LL, 66LL, 67LL}))), str(" "), str("\n"));    // 66
    total = 0LL;    // 68
    auto && __range_2 = bytes(std::string("abc", 3));    // 69
    for (auto &&__item_2 : __range_2) {    // 69
        byte = __item_2;    // 69
        total += byte;    // 70
    }    // 69
    print(tuple(total), str(" "), str("\n"));    // 71
    joined = bytes(std::string("", 0));    // 73
    auto && __range_3 = bytes(std::string("abc", 3));    // 74
    for (auto &&__item_3 : __range_3) {    // 74
        byte = __item_3;    // 74
        joined = ((joined + bytes(list<int64_t>({byte}))) + bytes(std::string(".", 1)));    // 75
    }    // 74
    print(tuple(joined), str(" "), str("\n"));    // 76
    return 0LL;    // 78
}    // 1
}

namespace prog_casts {

int run();    // 1

int run() {    // 1
    double a;    // 2
    int64_t b;    // 3
    double c;    // 4
    str float_str;    // 5
    double f1;    // 6
    str int_str;    // 7
    int64_t i1;    // 8
    int64_t i2;    // 9
    a = 2.0;    // 2
    b = to_int(a);    // 3
    c = to_float(b);    // 4
    float_str = str("  0.10 ");    // 5
    f1 = to_float(float_str);    // 6
    int_str = str("100");    // 7
    i1 = to_int(int_str);    // 8
    i2 = to_int(int_str, 2LL);    // 9
    print(tuple(a, b, c, f1, i1, i2), str(" "), str("\n"));    // 10
    return 0LL;    // 11
}    // 1
}

namespace prog_classes {

class Counter;    // 1
class Point;    // 14
class Empty;    // 39

int run();    // 43

class Counter {    // 1
  public:    // 1
    int64_t count;    // 1

    Counter(const int64_t &RESTRICT start) { __init__(start); }    // 1

    Counter() = default;    // 1

    void __init__(const int64_t &RESTRICT start);    // 4
    void bump(const int64_t &RESTRICT by);    // 7
    int64_t doubled();    // 10
};    // 10

class Point {    // 14
  public:    // 14
    int64_t x;    // 14
    int64_t y;    // 14

    Point(const int64_t &RESTRICT x, const int64_t &RESTRICT y) { __init__(x, y); }    // 14

    Point() = default;    // 14

    void __init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y);    // 18
    int64_t norm();    // 22
    Point moved(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy);    // 25
    str __str__();    // 29
    int64_t __len__();    // 32
    bool __bool__();    // 35
};    // 35

class Empty {    // 39
  public:    // 39
};    // 39

void Counter::__init__(const int64_t &RESTRICT start) {    // 4
    this->count = start;    // 5
}    // 4

void Counter::bump(const int64_t &RESTRICT by) {    // 7
    this->count = (this->count + by);    // 8
}    // 7

int64_t Counter::doubled() {    // 10
    return (this->count * 2LL);    // 11
}    // 10

void Point::__init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y) {    // 18
    this->x = x;    // 19
    this->y = y;    // 20
}    // 18

int64_t Point::norm() {    // 22
    return ((this->x * this->x) + (this->y * this->y));    // 23
}    // 22

Point Point::moved(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy) {    // 25
    return Point((this->x + dx), (this->y + dy));    // 26
}    // 25

str Point::__str__() {    // 29
    return ((((str("Point(") + to_str(this->x)) + str(", ")) + to_str(this->y)) + str(")"));    // 30
}    // 29

int64_t Point::__len__() {    // 32
    return 2LL;    // 33
}    // 32

bool Point::__bool__() {    // 35
    return _or(((this->x != 0LL)), ((this->y != 0LL)));    // 36
}    // 35

int run() {    // 43
    Counter counter;    // 44
    Point p;    // 48
    Point q;    // 57
    list<Point> points;    // 65
    Point point;    // 66
    Empty e;    // 70
    counter = Counter(5LL);    // 44
    counter.bump(3LL);    // 45
    print(tuple(counter.count, counter.doubled()), str(" "), str("\n"));    // 46
    p = Point(3LL, 4LL);    // 48
    print(tuple(p.x, p.y, p.norm()), str(" "), str("\n"));    // 49
    print(tuple(p), str(" "), str("\n"));    // 52
    print(tuple(len(p)), str(" "), str("\n"));    // 53
    print(tuple(to_bool(p), to_bool(Point(0LL, 0LL))), str(" "), str("\n"));    // 54
    q = p.moved(1LL, 1LL);    // 57
    print(tuple(q, q.norm()), str(" "), str("\n"));    // 58
    p.x = 10LL;    // 61
    print(tuple(p.x, p.norm()), str(" "), str("\n"));    // 62
    points = list<Point>({Point(1LL, 1LL), Point(2LL, 2LL)});    // 65
    auto && __range_4 = points;    // 66
    for (auto &&__item_4 : __range_4) {    // 66
        point = __item_4;    // 66
        print(tuple(point, point.norm()), str(" "), str("\n"));    // 67
    }    // 66
    print(tuple(len(points)), str(" "), str("\n"));    // 68
    e = Empty();    // 70
    return 0LL;    // 72
}    // 43
}

namespace prog_comparison {

int run();    // 1

int run() {    // 1
    int64_t a;    // 2
    int64_t b;    // 3
    int64_t c;    // 4
    int64_t d;    // 8
    a = 1LL;    // 2
    b = 2LL;    // 3
    c = 3LL;    // 4
    print(tuple(((a < b) && (b < c))), str(" "), str("\n"));    // 5
    print(tuple(((a > b))), str(" "), str("\n"));    // 6
    print(tuple(((a > b) && (b > c))), str(" "), str("\n"));    // 7
    d = 3LL;    // 8
    print(tuple(((c <= d))), str(" "), str("\n"));    // 9
    print(tuple(((a == d))), str(" "), str("\n"));    // 10
    print(tuple(((c == d))), str(" "), str("\n"));    // 11
    return 0LL;    // 12
}    // 1
}

namespace prog_comprehensions {

list<int64_t> __list_comprehension_1(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor, const int64_t &RESTRICT offset);    // 4
list<int64_t> scaled(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor);    // 1
list<int64_t> __list_comprehension_2(const list<int64_t> &RESTRICT numbers);    // 11
list<int64_t> __list_comprehension_3(const list<int64_t> &RESTRICT numbers);    // 12
list<int64_t> __list_comprehension_4();    // 15
list<int64_t> __list_comprehension_5();    // 16
list<int64_t> __list_comprehension_6(const list<int64_t> &RESTRICT numbers);    // 17
set<int64_t> __set_comprehension_0(const list<int64_t> &RESTRICT numbers);    // 23
dict<int64_t, int64_t> __dict_comprehension_0(const list<int64_t> &RESTRICT numbers);    // 25
list<int64_t> __list_comprehension_7(const list<int64_t> &RESTRICT numbers);    // 30
list<int64_t> __list_comprehension_11(const int64_t &RESTRICT v);    // 33
list<int64_t> __list_comprehension_8(const list<int64_t> &RESTRICT numbers);    // 33
list<int64_t> __list_comprehension_9(const list<int64_t> &RESTRICT numbers);    // 36
list<int64_t> __list_comprehension_10(const list<int64_t> &RESTRICT doubled);    // 37
int run();    // 7

list<int64_t> __list_comprehension_1(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor, const int64_t &RESTRICT offset) {    // 4
    list<int64_t> __tmp_1;    // 4
    int64_t v;    // 4
    __tmp_1 = list<int64_t>();    // 4
    auto && __range_5 = values;    // 4
    for (auto &&__item_5 : __range_5) {    // 4
        v = __item_5;    // 4
        __tmp_1.append(((v * factor) + offset));    // 4
    }    // 4
    return __tmp_1;    // 4
}    // 4

list<int64_t> scaled(const list<int64_t> &RESTRICT values, const int64_t &RESTRICT factor) {    // 1
    int64_t offset;    // 3
    offset = 1LL;    // 3
    return __list_comprehension_1(values, factor, offset);    // 4
}    // 1

list<int64_t> __list_comprehension_2(const list<int64_t> &RESTRICT numbers) {    // 11
    list<int64_t> __tmp_2;    // 11
    int64_t v;    // 11
    __tmp_2 = list<int64_t>();    // 11
    auto && __range_6 = numbers;    // 11
    for (auto &&__item_6 : __range_6) {    // 11
        v = __item_6;    // 11
        __tmp_2.append(v);    // 11
    }    // 11
    return __tmp_2;    // 11
}    // 11

list<int64_t> __list_comprehension_3(const list<int64_t> &RESTRICT numbers) {    // 12
    list<int64_t> __tmp_3;    // 12
    int64_t v;    // 12
    __tmp_3 = list<int64_t>();    // 12
    auto && __range_7 = numbers;    // 12
    for (auto &&__item_7 : __range_7) {    // 12
        v = __item_7;    // 12
        if (((v > 2LL))) {    // 12
            __tmp_3.append(v);    // 12
        }    // 12
    }    // 12
    return __tmp_3;    // 12
}    // 12

list<int64_t> __list_comprehension_4() {    // 15
    list<int64_t> __tmp_4;    // 15
    int64_t i;    // 15
    __tmp_4 = list<int64_t>();    // 15
    int64_t __stop_0 = 4LL;    // 15
    for (i = 0; i < __stop_0; ++i) {    // 15
        __tmp_4.append(i);    // 15
    }    // 15
    return __tmp_4;    // 15
}    // 15

list<int64_t> __list_comprehension_5() {    // 16
    list<int64_t> __tmp_5;    // 16
    int64_t i;    // 16
    __tmp_5 = list<int64_t>();    // 16
    int64_t __stop_1 = 8LL;    // 16
    for (i = 1LL; i < __stop_1; i += 2) {    // 16
        __tmp_5.append(i);    // 16
    }    // 16
    return __tmp_5;    // 16
}    // 16

list<int64_t> __list_comprehension_6(const list<int64_t> &RESTRICT numbers) {    // 17
    list<int64_t> __tmp_6;    // 17
    int64_t i;    // 17
    __tmp_6 = list<int64_t>();    // 17
    int64_t __len_0 = len(numbers);    // 17
    for (i = 0; i < __len_0; ++i) {    // 17
        __tmp_6.append(numbers[i]);    // 17
    }    // 17
    return __tmp_6;    // 17
}    // 17

set<int64_t> __set_comprehension_0(const list<int64_t> &RESTRICT numbers) {    // 23
    set<int64_t> __tmp_7;    // 23
    int64_t v;    // 23
    __tmp_7 = set<int64_t>();    // 23
    auto && __range_8 = numbers;    // 23
    for (auto &&__item_8 : __range_8) {    // 23
        v = __item_8;    // 23
        __tmp_7.add((v * v));    // 23
    }    // 23
    return __tmp_7;    // 23
}    // 23

dict<int64_t, int64_t> __dict_comprehension_0(const list<int64_t> &RESTRICT numbers) {    // 25
    dict<int64_t, int64_t> __tmp_8;    // 25
    int64_t v;    // 25
    __tmp_8 = dict<int64_t, int64_t>();    // 25
    auto && __range_9 = numbers;    // 25
    for (auto &&__item_9 : __range_9) {    // 25
        v = __item_9;    // 25
        if (((v > 1LL))) {    // 25
            __tmp_8[v] = (v * v);    // 25
        }    // 25
    }    // 25
    return __tmp_8;    // 25
}    // 25

list<int64_t> __list_comprehension_7(const list<int64_t> &RESTRICT numbers) {    // 30
    list<int64_t> __tmp_9;    // 30
    int64_t x;    // 30
    int64_t y;    // 30
    __tmp_9 = list<int64_t>();    // 30
    auto && __range_10 = numbers;    // 30
    for (auto &&__item_10 : __range_10) {    // 30
        x = __item_10;    // 30
        auto && __range_11 = numbers;    // 30
        for (auto &&__item_11 : __range_11) {    // 30
            y = __item_11;    // 30
            if (((x < y))) {    // 30
                __tmp_9.append((x * y));    // 30
            }    // 30
        }    // 30
    }    // 30
    return __tmp_9;    // 30
}    // 30

list<int64_t> __list_comprehension_11(const int64_t &RESTRICT v) {    // 33
    list<int64_t> __tmp_13;    // 33
    int64_t w;    // 33
    __tmp_13 = list<int64_t>();    // 33
    int64_t __stop_2 = v;    // 33
    for (w = 0; w < __stop_2; ++w) {    // 33
        __tmp_13.append(w);    // 33
    }    // 33
    return __tmp_13;    // 33
}    // 33

list<int64_t> __list_comprehension_8(const list<int64_t> &RESTRICT numbers) {    // 33
    list<int64_t> __tmp_10;    // 33
    int64_t v;    // 33
    __tmp_10 = list<int64_t>();    // 33
    auto && __range_12 = numbers;    // 33
    for (auto &&__item_12 : __range_12) {    // 33
        v = __item_12;    // 33
        __tmp_10.append(len(__list_comprehension_11(v)));    // 33
    }    // 33
    return __tmp_10;    // 33
}    // 33

list<int64_t> __list_comprehension_9(const list<int64_t> &RESTRICT numbers) {    // 36
    list<int64_t> __tmp_11;    // 36
    int64_t v;    // 36
    __tmp_11 = list<int64_t>();    // 36
    auto && __range_13 = numbers;    // 36
    for (auto &&__item_13 : __range_13) {    // 36
        v = __item_13;    // 36
        __tmp_11.append((v * 2LL));    // 36
    }    // 36
    return __tmp_11;    // 36
}    // 36

list<int64_t> __list_comprehension_10(const list<int64_t> &RESTRICT doubled) {    // 37
    list<int64_t> __tmp_12;    // 37
    int64_t v;    // 37
    __tmp_12 = list<int64_t>();    // 37
    auto && __range_14 = doubled;    // 37
    for (auto &&__item_14 : __range_14) {    // 37
        v = __item_14;    // 37
        __tmp_12.append((v + 1LL));    // 37
    }    // 37
    return __tmp_12;    // 37
}    // 37

int run() {    // 7
    list<int64_t> numbers;    // 8
    set<int64_t> squares;    // 23
    dict<int64_t, int64_t> lookup;    // 25
    list<int64_t> doubled;    // 36
    numbers = list<int64_t>({1LL, 2LL, 3LL, 4LL});    // 8
    print(tuple(__list_comprehension_2(numbers)), str(" "), str("\n"));    // 11
    print(tuple(__list_comprehension_3(numbers)), str(" "), str("\n"));    // 12
    print(tuple(__list_comprehension_4()), str(" "), str("\n"));    // 15
    print(tuple(__list_comprehension_5()), str(" "), str("\n"));    // 16
    print(tuple(__list_comprehension_6(numbers)), str(" "), str("\n"));    // 17
    print(tuple(scaled(numbers, 10LL)), str(" "), str("\n"));    // 20
    squares = __set_comprehension_0(numbers);    // 23
    print(tuple(sorted(squares, false)), str(" "), str("\n"));    // 24
    lookup = __dict_comprehension_0(numbers);    // 25
    print(tuple(len(lookup), lookup[2LL], lookup[4LL]), str(" "), str("\n"));    // 26
    print(tuple(sorted(lookup, false)), str(" "), str("\n"));    // 27
    print(tuple(__list_comprehension_7(numbers)), str(" "), str("\n"));    // 30
    print(tuple(__list_comprehension_8(numbers)), str(" "), str("\n"));    // 33
    doubled = __list_comprehension_9(numbers);    // 36
    print(tuple(__list_comprehension_10(doubled)), str(" "), str("\n"));    // 37
    return 0LL;    // 39
}    // 7
}

namespace prog_dict {

int run();    // 1

int run() {    // 1
    dict<int64_t, int64_t> d;    // 2
    dict<int64_t, int64_t> e;    // 32
    dict<int64_t, int64_t> c;    // 36
    dict<str, int64_t> s;    // 41
    d = dict<int64_t, int64_t>({{1LL, 10LL}, {2LL, 20LL}, {3LL, 30LL}});    // 2
    print(tuple(len(d)), str(" "), str("\n"));    // 6
    print(tuple(d[1LL], d[2LL], d[3LL]), str(" "), str("\n"));    // 7
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 8
    print(tuple(sorted(d, true)), str(" "), str("\n"));    // 9
    print(tuple(sorted(d, [](auto k) { return (-k); }, false)), str(" "), str("\n"));    // 10
    print(tuple(sorted(d, [](auto k) { return (-k); }, true)), str(" "), str("\n"));    // 11
    d[4LL] = 40LL;    // 13
    print(tuple(len(d), d[4LL]), str(" "), str("\n"));    // 14
    d[1LL] = 11LL;    // 15
    print(tuple(len(d), d[1LL]), str(" "), str("\n"));    // 16
    print(tuple(d.get(1LL)), str(" "), str("\n"));    // 18
    print(tuple(d.get(99LL, (-1LL))), str(" "), str("\n"));    // 19
    print(tuple(d.pop(4LL)), str(" "), str("\n"));    // 21
    print(tuple(d.pop(99LL, (-1LL))), str(" "), str("\n"));    // 22
    print(tuple(len(d)), str(" "), str("\n"));    // 23
    print(tuple(d.setdefault(2LL, 999LL)), str(" "), str("\n"));    // 25
    print(tuple(d.setdefault(9LL, 90LL)), str(" "), str("\n"));    // 26
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 27
    print(tuple(sorted(d.keys(), false)), str(" "), str("\n"));    // 29
    print(tuple(sorted(d.values(), false)), str(" "), str("\n"));    // 30
    e = dict<int64_t, int64_t>({{5LL, 50LL}});    // 32
    d.update(e);    // 33
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 34
    c = d.copy();    // 36
    print(tuple(len(c)), str(" "), str("\n"));    // 37
    c.clear();    // 38
    print(tuple(len(c), len(d)), str(" "), str("\n"));    // 39
    s = dict<str, int64_t>({{str("b"), 2LL}, {str("a"), 1LL}});    // 41
    print(tuple(sorted(s, false)), str(" "), str("\n"));    // 42
    print(tuple(s[str("a")], s[str("b")]), str(" "), str("\n"));    // 43
    return 0LL;    // 45
}    // 1
}

namespace prog_exceptions {

int64_t guarded_parse(const str &RESTRICT text);    // 1
int64_t parse_or(const str &RESTRICT text, const int64_t &RESTRICT fallback);    // 9
int64_t nested();    // 21
int64_t handler_raises();    // 34
int64_t check_positive(const int64_t &RESTRICT n);    // 51
int64_t reraise(const int64_t &RESTRICT n);    // 57
int64_t raise_bare_class(const str &RESTRICT key);    // 68
int64_t siblings(const str &RESTRICT first, const str &RESTRICT second);    // 72
int64_t relay();    // 89
int run();    // 98

int64_t guarded_parse(const str &RESTRICT text) {    // 1
    {    // 3
        Finally __finally([&] {    // 3
            print(tuple(str("cleanup"), text), str(" "), str("\n"));    // 6
        });    // 3
        return to_int(text);    // 4
    }    // 3
}    // 1

int64_t parse_or(const str &RESTRICT text, const int64_t &RESTRICT fallback) {    // 9
    int64_t value;    // 12
    {    // 11
        bool __thrown = false;    // 11
        try {    // 11
            value = to_int(text);    // 12
        } catch (ValueError &) {    // 11
            __thrown = true;    // 11
            print(tuple(str("bad literal"), text), str(" "), str("\n"));    // 14
            return fallback;    // 15
        }    // 11
        if (!__thrown) {    // 11
            print(tuple(str("good literal"), text), str(" "), str("\n"));    // 17
            return value;    // 18
        }    // 11
    }    // 11
}    // 9

int64_t nested() {    // 21
    {    // 22
        Finally __finally([&] {    // 22
            print(tuple(str("outer finally")), str(" "), str("\n"));    // 31
        });    // 22
        try {    // 22
            {    // 23
                Finally __finally([&] {    // 23
                    print(tuple(str("inner finally")), str(" "), str("\n"));    // 26
                });    // 23
                return to_int(str("nope"));    // 24
            }    // 23
        } catch (ValueError &) {    // 22
            print(tuple(str("outer caught")), str(" "), str("\n"));    // 28
            return (-1LL);    // 29
        }    // 22
    }    // 22
}    // 21

int64_t handler_raises() {    // 34
    try {    // 36
        {    // 37
            Finally __finally([&] {    // 37
                print(tuple(str("guard ran")), str(" "), str("\n"));    // 45
            });    // 37
            bool __thrown = false;    // 37
            try {    // 37
                print(tuple(to_int(str("bad"))), str(" "), str("\n"));    // 38
            } catch (ValueError &) {    // 37
                __thrown = true;    // 37
                print(tuple(str("handler raising")), str(" "), str("\n"));    // 40
                return to_int(str("worse"));    // 41
            }    // 37
            if (!__thrown) {    // 37
                print(tuple(str("not reached")), str(" "), str("\n"));    // 43
            }    // 37
        }    // 37
    } catch (ValueError &) {    // 36
        print(tuple(str("caught the handler's exception")), str(" "), str("\n"));    // 47
    }    // 36
    return 0LL;    // 48
}    // 34

int64_t check_positive(const int64_t &RESTRICT n) {    // 51
    if (((n < 0LL))) {    // 52
        throw ValueError(str("negative"));    // 53
    }    // 52
    return n;    // 54
}    // 51

int64_t reraise(const int64_t &RESTRICT n) {    // 57
    {    // 59
        Finally __finally([&] {    // 59
            print(tuple(str("reraise finally")), str(" "), str("\n"));    // 65
        });    // 59
        try {    // 59
            return check_positive(n);    // 60
        } catch (ValueError &) {    // 59
            print(tuple(str("logging and passing it on")), str(" "), str("\n"));    // 62
            throw;    // 63
        }    // 59
    }    // 59
}    // 57

int64_t raise_bare_class(const str &RESTRICT key) {    // 68
    throw KeyError("");    // 69
}    // 68

int64_t siblings(const str &RESTRICT first, const str &RESTRICT second) {    // 72
    {    // 74
        bool __thrown = false;    // 74
        try {    // 74
            print(tuple(to_int(first)), str(" "), str("\n"));    // 75
        } catch (ValueError &) {    // 74
            __thrown = true;    // 74
            print(tuple(str("first was bad")), str(" "), str("\n"));    // 77
        }    // 74
        if (!__thrown) {    // 74
            print(tuple(str("first was fine")), str(" "), str("\n"));    // 79
        }    // 74
    }    // 74
    {    // 80
        bool __thrown = false;    // 80
        try {    // 80
            print(tuple(to_int(second)), str(" "), str("\n"));    // 81
        } catch (ValueError &) {    // 80
            __thrown = true;    // 80
            print(tuple(str("second was bad")), str(" "), str("\n"));    // 83
        }    // 80
        if (!__thrown) {    // 80
            print(tuple(str("second was fine")), str(" "), str("\n"));    // 85
        }    // 80
    }    // 80
    return 0LL;    // 86
}    // 72

int64_t relay() {    // 89
    try {    // 91
        throw ValueError(str("original"));    // 92
    } catch (PyException &e) {    // 91
        print(tuple(str("relaying")), str(" "), str("\n"));    // 94
        e.raise();    // 95
    }    // 91
}    // 89

int run() {    // 98
    list<int64_t> numbers;    // 139
    list<int64_t> items;    // 147
    int64_t i;    // 159
    print(tuple(guarded_parse(str("41"))), str(" "), str("\n"));    // 99
    try {    // 100
        print(tuple(guarded_parse(str("zzz"))), str(" "), str("\n"));    // 101
    } catch (ValueError &) {    // 100
        print(tuple(str("caught from callee")), str(" "), str("\n"));    // 103
    }    // 100
    print(tuple(parse_or(str("7"), 0LL)), str(" "), str("\n"));    // 105
    print(tuple(parse_or(str("seven"), 0LL)), str(" "), str("\n"));    // 106
    print(tuple(nested()), str(" "), str("\n"));    // 107
    print(tuple(handler_raises()), str(" "), str("\n"));    // 108
    print(tuple(check_positive(3LL)), str(" "), str("\n"));    // 110
    try {    // 111
        print(tuple(check_positive((-1LL))), str(" "), str("\n"));    // 112
    } catch (ValueError &) {    // 111
        print(tuple(str("caught the raise")), str(" "), str("\n"));    // 114
    }    // 111
    try {    // 116
        print(tuple(reraise((-2LL))), str(" "), str("\n"));    // 117
    } catch (ValueError &) {    // 116
        print(tuple(str("caught the re-raise")), str(" "), str("\n"));    // 119
    }    // 116
    try {    // 121
        print(tuple(raise_bare_class(str("k"))), str(" "), str("\n"));    // 122
    } catch (KeyError &) {    // 121
        print(tuple(str("caught the bare class")), str(" "), str("\n"));    // 124
    }    // 121
    print(tuple(siblings(str("8"), str("eight"))), str(" "), str("\n"));    // 126
    try {    // 128
        print(tuple(relay()), str(" "), str("\n"));    // 129
    } catch (ValueError &) {    // 128
        print(tuple(str("still a ValueError after the relay")), str(" "), str("\n"));    // 131
    }    // 128
    try {    // 134
        throw TypeError(str("wrong type"));    // 135
    } catch (PyException &) {    // 134
        print(tuple(str("base handler took the subclass")), str(" "), str("\n"));    // 137
    }    // 134
    numbers = list<int64_t>({1LL, 2LL, 3LL});    // 139
    try {    // 140
        print(tuple(numbers[10LL]), str(" "), str("\n"));    // 141
    } catch (IndexError &) {    // 140
        print(tuple(str("index error wins over the base class")), str(" "), str("\n"));    // 143
    } catch (PyException &) {    // 140
        print(tuple(str("not reached")), str(" "), str("\n"));    // 145
    }    // 140
    items = list<int64_t>({1LL, 2LL, 3LL});    // 147
    try {    // 148
        print(tuple(items[10LL]), str(" "), str("\n"));    // 149
    } catch (PyException &) {    // 148
        print(tuple(str("bare except caught it")), str(" "), str("\n"));    // 151
    }    // 148
    try {    // 153
        print(tuple(to_float(str("x"))), str(" "), str("\n"));    // 154
    } catch (ValueError &e) {    // 153
        print(tuple(str("float refused it")), str(" "), str("\n"));    // 156
    }    // 153
    int64_t __stop_3 = 3LL;    // 159
    for (i = 0; i < __stop_3; ++i) {    // 159
        {    // 160
            Finally __finally([&] {    // 160
                print(tuple(str("loop finally"), i), str(" "), str("\n"));    // 165
            });    // 160
            if (((i == 1LL))) {    // 161
                break;    // 162
            }    // 161
            print(tuple(str("loop"), i), str(" "), str("\n"));    // 163
        }    // 160
    }    // 159
    return 0LL;    // 167
}    // 98
}

namespace prog_files {

int run();    // 1

int run() {    // 1
    str SAMPLE;    // 2
    file handle;    // 3
    str text;    // 4
    file stepped;    // 12
    list<str> rest;    // 15
    file lines;    // 20
    str line;    // 21
    file out;    // 25
    str written;    // 29
    SAMPLE = str("tests/test_files/sample.txt");    // 2
    handle = open(SAMPLE);    // 3
    text = handle.read();    // 4
    print(tuple(len(text)), str(" "), str("\n"));    // 5
    print(tuple(text.splitlines()), str(" "), str("\n"));    // 6
    print(tuple(len(handle.read())), str(" "), str("\n"));    // 8
    handle.close();    // 9
    stepped = open(SAMPLE);    // 12
    print(tuple(stepped.readline().strip()), str(" "), str("\n"));    // 13
    print(tuple(stepped.readline().strip()), str(" "), str("\n"));    // 14
    rest = stepped.readlines();    // 15
    print(tuple(len(rest), rest[0LL].strip()), str(" "), str("\n"));    // 16
    print(tuple(len(stepped.readlines())), str(" "), str("\n"));    // 17
    lines = open(SAMPLE);    // 20
    auto && __range_15 = lines;    // 21
    for (auto &&__item_15 : __range_15) {    // 21
        line = __item_15;    // 21
        print(tuple(len(line), line.strip()), str(" "), str("\n"));    // 22
    }    // 21
    out = open(str("tests/test_files/sample_out.txt"), str("w"));    // 25
    print(tuple(out.write(str("alpha\n"))), str(" "), str("\n"));    // 26
    print(tuple(out.write(str("beta\n"))), str(" "), str("\n"));    // 27
    out.close();    // 28
    written = open(str("tests/test_files/sample_out.txt")).read();    // 29
    print(tuple(written.splitlines(), len(written)), str(" "), str("\n"));    // 30
    try {    // 32
        open(str("tests/test_files/no_such_file.txt"));    // 33
    } catch (FileNotFoundError &) {    // 32
        print(tuple(str("missing file raised")), str(" "), str("\n"));    // 35
    }    // 32
    return 0LL;    // 37
}    // 1
}

namespace prog_fixed_width_int_promotion {

uint16_t scale(const uint16_t &RESTRICT value, const uint16_t &RESTRICT factor);    // 4
int32_t clamp(const int32_t &RESTRICT value, const int32_t &RESTRICT low, const int32_t &RESTRICT high);    // 8
int run();    // 16

uint16_t scale(const uint16_t &RESTRICT value, const uint16_t &RESTRICT factor) {    // 4
    return (value * factor);    // 5
}    // 4

int32_t clamp(const int32_t &RESTRICT value, const int32_t &RESTRICT low, const int32_t &RESTRICT high) {    // 8
    if (((value < low))) {    // 9
        return low;    // 10
    }    // 9
    if (((value > high))) {    // 11
        return high;    // 12
    }    // 11
    return value;    // 13
}    // 8

int run() {    // 16
    uint16_t n;    // 18
    uint8_t m;    // 19
    uint16_t doubled;    // 23
    uint16_t total;    // 28
    n = 10LL;    // 18
    m = 200LL;    // 19
    print(tuple(n, m), str(" "), str("\n"));    // 20
    doubled = scale(n, 2LL);    // 23
    print(tuple(doubled), str(" "), str("\n"));    // 24
    total = (scale(n, 2LL) + n);    // 28
    print(tuple(total), str(" "), str("\n"));    // 29
    print(tuple(clamp(500LL, 0LL, 100LL)), str(" "), str("\n"));    // 31
    print(tuple(clamp((-5LL), 0LL, 100LL)), str(" "), str("\n"));    // 32
    print(tuple(clamp(50LL, 0LL, 100LL)), str(" "), str("\n"));    // 33
    return 0LL;    // 35
}    // 16
}

namespace prog_fixed_width_ints {

uint16_t add_u16(const uint16_t &RESTRICT a, const uint16_t &RESTRICT b);    // 4
int run();    // 8

uint16_t add_u16(const uint16_t &RESTRICT a, const uint16_t &RESTRICT b) {    // 4
    return uint16_t((a + b));    // 5
}    // 4

int run() {    // 8
    uint8_t a;    // 9
    int8_t b;    // 10
    uint16_t c;    // 13
    uint16_t d;    // 14
    int32_t e;    // 17
    uint32_t f;    // 18
    int64_t g;    // 21
    uint64_t h;    // 22
    list<uint16_t> values;    // 28
    dict<uint8_t, str> counts;    // 31
    set<int32_t> seen;    // 36
    a = uint8_t(200LL);    // 9
    b = int8_t((-100LL));    // 10
    print(tuple(a, b), str(" "), str("\n"));    // 11
    c = uint16_t(1000LL);    // 13
    d = add_u16(c, uint16_t(234LL));    // 14
    print(tuple(d), str(" "), str("\n"));    // 15
    e = int32_t((-70000LL));    // 17
    f = uint32_t(70000LL);    // 18
    print(tuple(e, f), str(" "), str("\n"));    // 19
    g = int64_t((-5000000000LL));    // 21
    h = uint64_t(5000000000LL);    // 22
    print(tuple(g, h), str(" "), str("\n"));    // 23
    print(tuple(((uint16_t(5LL) == uint16_t(5LL)))), str(" "), str("\n"));    // 25
    print(tuple(((uint16_t(5LL) < uint16_t(10LL)))), str(" "), str("\n"));    // 26
    values = list<uint16_t>({uint16_t(3LL), uint16_t(1LL), uint16_t(2LL)});    // 28
    print(tuple(values), str(" "), str("\n"));    // 29
    counts = dict<uint8_t, str>();    // 31
    counts[uint8_t(1LL)] = str("one");    // 32
    counts[uint8_t(2LL)] = str("two");    // 33
    print(tuple(counts[uint8_t(1LL)], counts[uint8_t(2LL)]), str(" "), str("\n"));    // 34
    seen = set<int32_t>({int32_t(1LL), int32_t(2LL), int32_t(2LL)});    // 36
    print(tuple(len(seen)), str(" "), str("\n"));    // 37
    return 0LL;    // 39
}    // 8
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

int run();    // 19

int run() {    // 19
    Array<uint8_t, 24> arr;    // 24
    print(tuple(str("Test 1 - derived global:"), totalSlots), str(" "), str("\n"));    // 21
    arr = Array<uint8_t, 24>();    // 24
    print(tuple(str("Test 2 - array sized by derived global:"), len(arr)), str(" "), str("\n"));    // 25
    print(tuple(str("Test 3 - shift masks:"), pair0_mask, pair2_mask), str(" "), str("\n"));    // 28
    return 0LL;    // 30
}    // 19
}

namespace prog_hotpath {

class Vector;    // 63

int64_t clamp(const int64_t &RESTRICT x, const int64_t &RESTRICT low, const int64_t &RESTRICT high);    // 4
int64_t normalize_price(const int64_t &RESTRICT raw, const int64_t &RESTRICT tick_size);    // 12
FLATTEN int64_t process_order(const int64_t &RESTRICT bid, const int64_t &RESTRICT ask, const int64_t &RESTRICT qty, const int64_t &RESTRICT tick_size);    // 20
int64_t square(const int64_t &RESTRICT x);    // 40
int64_t add(const int64_t &RESTRICT a, const int64_t &RESTRICT b);    // 44
FLATTEN int64_t distance_squared(const int64_t &RESTRICT x1, const int64_t &RESTRICT y1, const int64_t &RESTRICT x2, const int64_t &RESTRICT y2);    // 49
FLATTEN int64_t sum_up_to(const int64_t &RESTRICT n);    // 56
int run();    // 83

class Vector {    // 63
  public:    // 63
    int64_t x;    // 63
    int64_t y;    // 63

    Vector(const int64_t &RESTRICT x, const int64_t &RESTRICT y) { __init__(x, y); }    // 63

    Vector() = default;    // 63

    void __init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y);    // 64
    Vector scaled(const int64_t &RESTRICT factor);    // 68
    FLATTEN int64_t length_squared();    // 72
    FLATTEN void move(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy);    // 77
};    // 77

void Vector::__init__(const int64_t &RESTRICT x, const int64_t &RESTRICT y) {    // 64
    this->x = x;    // 65
    this->y = y;    // 66
}    // 64

Vector Vector::scaled(const int64_t &RESTRICT factor) {    // 68
    return Vector((this->x * factor), (this->y * factor));    // 69
}    // 68

FLATTEN int64_t Vector::length_squared() {    // 72
    return distance_squared(0LL, 0LL, this->x, this->y);    // 74
}    // 72

FLATTEN void Vector::move(const int64_t &RESTRICT dx, const int64_t &RESTRICT dy) {    // 77
    this->x = clamp((this->x + dx), (-100LL), 100LL);    // 79
    this->y = clamp((this->y + dy), (-100LL), 100LL);    // 80
}    // 77

int64_t clamp(const int64_t &RESTRICT x, const int64_t &RESTRICT low, const int64_t &RESTRICT high) {    // 4
    if (((x < low))) {    // 5
        return low;    // 6
    }    // 5
    if (((x > high))) {    // 7
        return high;    // 8
    }    // 7
    return x;    // 9
}    // 4

int64_t normalize_price(const int64_t &RESTRICT raw, const int64_t &RESTRICT tick_size) {    // 12
    int64_t rounded;    // 15
    rounded = (idiv(raw, tick_size) * tick_size);    // 15
    return clamp(rounded, 0LL, 1000000LL);    // 16
}    // 12

FLATTEN int64_t process_order(const int64_t &RESTRICT bid, const int64_t &RESTRICT ask, const int64_t &RESTRICT qty, const int64_t &RESTRICT tick_size) {    // 20
    int64_t norm_bid;    // 24
    int64_t norm_ask;    // 25
    int64_t spread;    // 27
    int64_t total;    // 31
    norm_bid = normalize_price(bid, tick_size);    // 24
    norm_ask = normalize_price(ask, tick_size);    // 25
    spread = (norm_ask - norm_bid);    // 27
    if (((spread < 0LL))) {    // 28
        spread = 0LL;    // 29
    }    // 28
    total = (spread * qty);    // 31
    if (((qty > 0LL))) {    // 32
        total = (total + qty);    // 33
    } else {    // 32
        total = (total - qty);    // 35
    }    // 32
    return total;    // 37
}    // 20

int64_t square(const int64_t &RESTRICT x) {    // 40
    return (x * x);    // 41
}    // 40

int64_t add(const int64_t &RESTRICT a, const int64_t &RESTRICT b) {    // 44
    return (a + b);    // 45
}    // 44

FLATTEN int64_t distance_squared(const int64_t &RESTRICT x1, const int64_t &RESTRICT y1, const int64_t &RESTRICT x2, const int64_t &RESTRICT y2) {    // 49
    int64_t dx;    // 50
    int64_t dy;    // 51
    dx = square((x2 - x1));    // 50
    dy = square((y2 - y1));    // 51
    return add(dx, dy);    // 52
}    // 49

FLATTEN int64_t sum_up_to(const int64_t &RESTRICT n) {    // 56
    int64_t total;    // 57
    int64_t i;    // 58
    total = 0LL;    // 57
    int64_t __stop_4 = n;    // 58
    for (i = 0; i < __stop_4; ++i) {    // 58
        total = add(total, i);    // 59
    }    // 58
    return total;    // 60
}    // 56

int run() {    // 83
    Vector v;    // 96
    Vector scaled;    // 104
    print(tuple(str("Test 1 - process_order:"), process_order(10007LL, 10023LL, 5LL, 5LL)), str(" "), str("\n"));    // 86
    print(tuple(str("Test 1b - process_order (sell):"), process_order(9998LL, 10001LL, (-3LL), 5LL)), str(" "), str("\n"));    // 87
    print(tuple(str("Test 2 - distance_squared:"), distance_squared(0LL, 0LL, 3LL, 4LL)), str(" "), str("\n"));    // 90
    print(tuple(str("Test 3 - sum_up_to:"), sum_up_to(5LL)), str(" "), str("\n"));    // 93
    v = Vector(3LL, 4LL);    // 96
    print(tuple(str("Test 4 - length_squared:"), v.length_squared()), str(" "), str("\n"));    // 97
    v.move(200LL, (-200LL));    // 100
    print(tuple(str("Test 5 - move (clamped):"), v.x, v.y), str(" "), str("\n"));    // 101
    scaled = v.scaled(2LL);    // 104
    print(tuple(str("Test 6 - scaled:"), scaled.x, scaled.y), str(" "), str("\n"));    // 105
    return 0LL;    // 107
}    // 83
}

namespace prog_iter {

int run();    // 120

int run() {    // 120
    list<int64_t> nums;    // 121
    list<int64_t> a;    // 122
    str s;    // 124
    list<int64_t> filtered;    // 126
    int64_t x;    // 128
    int64_t y;    // 128
    int64_t i;    // 131
    int64_t n;    // 131
    nums = list<int64_t>({1LL, 2LL, 3LL, 4LL, 5LL});    // 121
    a = list<int64_t>(map([](auto x) { return (x * 2LL); }, nums));    // 122
    print(tuple(a), str(" "), str("\n"));    // 123
    auto && __range_16 = map([](auto x) { return to_str(x); }, nums);    // 124
    for (auto &&__item_16 : __range_16) {    // 124
        s = __item_16;    // 124
        print(tuple(s), str(" "), str("\n"));    // 125
    }    // 124
    filtered = list<int64_t>(filter([](auto x) { return ((mod(x, 2LL) == 0LL)); }, nums));    // 126
    print(tuple(filtered), str(" "), str("\n"));    // 127
    auto && __range_17 = zip(nums, a);    // 128
    for (auto &&__item_17 : __range_17) {    // 128
        destructure(x, y) = __item_17;    // 128
        print(tuple(x, y), str(" "), str("\n"));    // 129
    }    // 128
    nums = copy(a);    // 130
    auto && __range_18 = enumerate(nums);    // 131
    for (auto &&__item_18 : __range_18) {    // 131
        destructure(i, n) = __item_18;    // 131
        print(tuple(i, n), str(" "), str("\n"));    // 132
    }    // 131
    return 0LL;    // 133
}    // 120
}

namespace prog_list {

list<int64_t> give_list(list<int64_t> &RESTRICT l);    // 4
int run();    // 11

list<int64_t> give_list(list<int64_t> &RESTRICT l) {    // 4
    print(tuple(l), str(" "), str("\n"));    // 5
    l.append(2LL);    // 6
    print(tuple(l), str(" "), str("\n"));    // 7
    return copy(l);    // 8
}    // 4

int run() {    // 11
    list<int64_t> l;    // 13
    int64_t a;    // 23
    list<int64_t> l2;    // 30
    int64_t x;    // 47
    int64_t y;    // 49
    int64_t z;    // 51
    list<int64_t> l3;    // 57
    int64_t n;    // 89
    list<int64_t> l4;    // 94
    list<int64_t> l5;    // 100
    print(tuple(list<int64_t>(list<int64_t>({1LL, 2LL, 3LL}))), str(" "), str("\n"));    // 12
    l = list<int64_t>({1LL, 2LL, 3LL});    // 13
    print(tuple(l), str(" "), str("\n"));    // 14
    l = list<int64_t>({1LL, 2LL, 3LL});    // 15
    print(tuple(l), str(" "), str("\n"));    // 16
    l.append(4LL);    // 17
    print(tuple(l), str(" "), str("\n"));    // 18
    l = give_list(l);    // 19
    print(tuple(l), str(" "), str("\n"));    // 20
    print(tuple(l[1LL]), str(" "), str("\n"));    // 22
    a = l[0LL];    // 23
    print(tuple(l), str(" "), str("\n"));    // 24
    l[0LL] = a;    // 25
    print(tuple(l), str(" "), str("\n"));    // 26
    l[0LL] = 2LL;    // 27
    print(tuple(l), str(" "), str("\n"));    // 28
    l2 = copy(l[slice(0LL, 1LL, std::nullopt)]);    // 30
    print(tuple(l2), str(" "), str("\n"));    // 31
    l.insert(0LL, 100LL);    // 33
    print(tuple(l), str(" "), str("\n"));    // 34
    l.insert(2LL, 200LL);    // 35
    print(tuple(l), str(" "), str("\n"));    // 36
    l.insert((-1LL), 300LL);    // 37
    print(tuple(l), str(" "), str("\n"));    // 38
    l.insert(100LL, 400LL);    // 39
    print(tuple(l), str(" "), str("\n"));    // 40
    l.insert((-100LL), 500LL);    // 41
    print(tuple(l), str(" "), str("\n"));    // 42
    l.remove(200LL);    // 44
    print(tuple(l), str(" "), str("\n"));    // 45
    x = l.pop();    // 47
    print(tuple(x, l), str(" "), str("\n"));    // 48
    y = l.pop(0LL);    // 49
    print(tuple(y, l), str(" "), str("\n"));    // 50
    z = l.pop((-2LL));    // 51
    print(tuple(z, l), str(" "), str("\n"));    // 52
    l.extend(list<int64_t>({7LL, 8LL}));    // 54
    print(tuple(l), str(" "), str("\n"));    // 55
    l3 = l.copy();    // 57
    print(tuple(l3), str(" "), str("\n"));    // 58
    l.clear();    // 60
    print(tuple(l), str(" "), str("\n"));    // 61
    l2 = list<int64_t>({5LL, 3LL, 1LL, 3LL, 9LL});    // 63
    print(tuple(l2.index(3LL)), str(" "), str("\n"));    // 64
    print(tuple(l2.index(3LL, 3LL)), str(" "), str("\n"));    // 65
    print(tuple(l2.index(3LL, 0LL, 2LL)), str(" "), str("\n"));    // 66
    print(tuple(l2.index(9LL, (-2LL))), str(" "), str("\n"));    // 67
    print(tuple(l2.count(3LL)), str(" "), str("\n"));    // 68
    print(tuple(l2.count(42LL)), str(" "), str("\n"));    // 69
    l2.sort(false);    // 71
    print(tuple(l2), str(" "), str("\n"));    // 72
    l2.sort(true);    // 73
    print(tuple(l2), str(" "), str("\n"));    // 74
    l2.sort(false);    // 75
    print(tuple(l2), str(" "), str("\n"));    // 76
    print(tuple(sorted(l2, [](auto x) { return (-x); }, false)), str(" "), str("\n"));    // 78
    print(tuple(sorted(l2, [](auto x) { return (-x); }, true)), str(" "), str("\n"));    // 79
    l2.sort([](auto x) { return (-x); }, false);    // 81
    print(tuple(l2), str(" "), str("\n"));    // 82
    l2.sort([](auto x) { return (-x); }, true);    // 83
    print(tuple(l2), str(" "), str("\n"));    // 84
    l2.reverse();    // 86
    print(tuple(l2), str(" "), str("\n"));    // 87
    n = len(l2);    // 89
    print(tuple(n), str(" "), str("\n"));    // 90
    print(tuple(l2[0LL], l2.back()), str(" "), str("\n"));    // 91
    if (!(((l2.back() == l2[(n - 1LL)])))) throw AssertionError("");    // 92
    l4 = list<int64_t>({1LL, 2LL, 3LL});    // 94
    print(tuple(l4.back()), str(" "), str("\n"));    // 95
    l4.back() += 10LL;    // 96
    if (!(((l4.back() == 13LL)))) throw AssertionError("");    // 97
    print(tuple(l4), str(" "), str("\n"));    // 98
    l5 = (list<int64_t>({1LL, 2LL}) * 3LL);    // 100
    print(tuple(l5), str(" "), str("\n"));    // 101
    l5 = (3LL * list<int64_t>({1LL, 2LL}));    // 102
    print(tuple(l5), str(" "), str("\n"));    // 103
    if (!(((l5 == (list<int64_t>({1LL, 2LL}) * 3LL))))) throw AssertionError("");    // 104
    l5 *= 2LL;    // 105
    print(tuple(l5), str(" "), str("\n"));    // 106
    return 0LL;    // 108
}    // 11
}

namespace prog_loops {

int run();    // 1

int run() {    // 1
    int64_t x;    // 2
    list<int64_t> l;    // 3
    int64_t i;    // 4
    int64_t step;    // 14
    int64_t n;    // 20
    x = 2LL;    // 2
    l = list<int64_t>({2LL, 3LL, 4LL});    // 3
    int64_t __len_1 = len(l);    // 4
    for (i = 0; i < __len_1; ++i) {    // 4
        print(tuple(l), str(" "), str("\n"));    // 5
    }    // 4
    int64_t __stop_5 = x;    // 6
    for (i = 0; i < __stop_5; ++i) {    // 6
        print(tuple(str("first"), i), str(" "), str("\n"));    // 7
    }    // 6
    int64_t __stop_6 = (x + 5LL);    // 8
    for (i = x; i < __stop_6; ++i) {    // 8
        print(tuple(str("second"), i), str(" "), str("\n"));    // 9
    }    // 8
    int64_t __stop_7 = (x + 10LL);    // 10
    for (i = x; i < __stop_7; i += 2) {    // 10
        print(tuple(str("third"), i), str(" "), str("\n"));    // 11
    }    // 10
    int64_t __stop_8 = (x - 7LL);    // 12
    for (i = x; i > __stop_8; i += -2) {    // 12
        print(tuple(str("fourth"), i), str(" "), str("\n"));    // 13
    }    // 12
    step = x;    // 14
    int64_t __stop_9 = (10LL * x);    // 15
    int64_t __step_0 = step;    // 15
    for (i = x;; i += __step_0) {    // 15
        if ((__step_0 > 0 && i >= __stop_9) || (__step_0 < 0 && i <= __stop_9)) break;    // 15
        print(tuple(str("fifth"), i), str(" "), str("\n"));    // 16
    }    // 15
    step = (-2LL);    // 17
    int64_t __stop_10 = (10LL * x);    // 18
    int64_t __step_1 = step;    // 18
    for (i = (5LL * x);; i += __step_1) {    // 18
        if ((__step_1 > 0 && i >= __stop_10) || (__step_1 < 0 && i <= __stop_10)) break;    // 18
        print(tuple(str("sixth"), i), str(" "), str("\n"));    // 19
    }    // 18
    auto && __range_19 = l;    // 20
    for (auto &&__item_19 : __range_19) {    // 20
        n = __item_19;    // 20
        print(tuple(str("seventh"), n), str(" "), str("\n"));    // 21
    }    // 20
    auto && __range_20 = l;    // 22
    for (auto &&__item_20 : __range_20) {    // 22
        n = __item_20;    // 22
        print(tuple(str("eight"), n), str(" "), str("\n"));    // 23
    }    // 22
    return 0LL;    // 24
}    // 1
}

namespace prog_math {

int run();    // 1

int run() {    // 1
    int64_t a;    // 8
    print(tuple(pow(10LL, 10LL)), str(" "), str("\n"));    // 2
    print(tuple(idiv((-10LL), 3LL)), str(" "), str("\n"));    // 3
    print(tuple(idiv(10LL, 3LL)), str(" "), str("\n"));    // 4
    print(tuple(fdiv(5LL, 2LL)), str(" "), str("\n"));    // 5
    print(tuple(pow(0.5, 4LL)), str(" "), str("\n"));    // 6
    print(tuple((50.0 * 100LL)), str(" "), str("\n"));    // 7
    a = pow(10LL, 10LL);    // 8
    print(tuple((-(-5LL))), str(" "), str("\n"));    // 9
    print(tuple((~5LL)), str(" "), str("\n"));    // 10
    print(tuple((-(+5LL))), str(" "), str("\n"));    // 11
    return a;    // 12
}    // 1
}

namespace prog_membership {

int run();    // 1

int run() {    // 1
    list<int64_t> l;    // 2
    dict<int64_t, int64_t> d;    // 6
    set<int64_t> s;    // 12
    str text;    // 16
    tuple<int64_t, int64_t, int64_t> t;    // 21
    list<tuple<int64_t, int64_t>> pairs;    // 26
    set<tuple<int64_t, int64_t>> pair_set;    // 32
    dict<tuple<int64_t, int64_t>, str> pair_dict;    // 36
    tuple<tuple<int64_t, int64_t>, tuple<int64_t, int64_t>> nested;    // 39
    list<tuple<str, int64_t>> mixed;    // 42
    list<str> strs;    // 45
    int64_t count;    // 54
    int64_t x;    // 55
    l = list<int64_t>({1LL, 2LL, 3LL});    // 2
    print(tuple((l.__contains__(2LL)), (l.__contains__(9LL))), str(" "), str("\n"));    // 3
    print(tuple((!l.__contains__(2LL)), (!l.__contains__(9LL))), str(" "), str("\n"));    // 4
    d = dict<int64_t, int64_t>({{1LL, 10LL}, {2LL, 20LL}});    // 6
    print(tuple((d.__contains__(1LL)), (d.__contains__(9LL))), str(" "), str("\n"));    // 8
    print(tuple((d.values().__contains__(10LL))), str(" "), str("\n"));    // 9
    print(tuple((!d.__contains__(1LL)), (!d.__contains__(9LL))), str(" "), str("\n"));    // 10
    s = set<int64_t>({1LL, 2LL, 3LL});    // 12
    print(tuple((s.__contains__(2LL)), (s.__contains__(9LL))), str(" "), str("\n"));    // 13
    print(tuple((!s.__contains__(2LL)), (!s.__contains__(9LL))), str(" "), str("\n"));    // 14
    text = str("hello world");    // 16
    print(tuple((text.__contains__(str("hello"))), (text.__contains__(str("zz")))), str(" "), str("\n"));    // 17
    print(tuple((text.__contains__(str("o w")))), str(" "), str("\n"));    // 18
    print(tuple((!text.__contains__(str("hello"))), (!text.__contains__(str("zz")))), str(" "), str("\n"));    // 19
    t = tuple(1LL, 2LL, 3LL);    // 21
    print(tuple((t.__contains__(2LL)), (t.__contains__(9LL))), str(" "), str("\n"));    // 22
    pairs = list<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});    // 26
    print(tuple((pairs.__contains__(tuple(1LL, 2LL))), (pairs.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));    // 27
    print(tuple((!pairs.__contains__(tuple(1LL, 2LL))), (!pairs.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));    // 28
    print(tuple((pairs.__contains__(tuple(2LL, 1LL)))), str(" "), str("\n"));    // 30
    pair_set = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});    // 32
    print(tuple((pair_set.__contains__(tuple(1LL, 2LL))), (pair_set.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));    // 33
    print(tuple((pair_set.__contains__(tuple(2LL, 1LL)))), str(" "), str("\n"));    // 34
    pair_dict = dict<tuple<int64_t, int64_t>, str>({{tuple(1LL, 2LL), str("a")}, {tuple(3LL, 4LL), str("b")}});    // 36
    print(tuple((pair_dict.__contains__(tuple(1LL, 2LL))), (pair_dict.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));    // 37
    nested = tuple(tuple(1LL, 2LL), tuple(3LL, 4LL));    // 39
    print(tuple((nested.__contains__(tuple(1LL, 2LL))), (nested.__contains__(tuple(9LL, 9LL)))), str(" "), str("\n"));    // 40
    mixed = list<tuple<str, int64_t>>({tuple(str("a"), 1LL), tuple(str("b"), 2LL)});    // 42
    print(tuple((mixed.__contains__(tuple(str("a"), 1LL))), (mixed.__contains__(tuple(str("a"), 2LL)))), str(" "), str("\n"));    // 43
    strs = list<str>({str("a"), str("b")});    // 45
    print(tuple((strs.__contains__(str("a"))), (strs.__contains__(str("z")))), str(" "), str("\n"));    // 46
    if ((l.__contains__(2LL))) {    // 49
        print(tuple(str("found")), str(" "), str("\n"));    // 50
    } else {    // 49
        print(tuple(str("missing")), str(" "), str("\n"));    // 52
    }    // 49
    count = 0LL;    // 54
    auto && __range_21 = list<int64_t>({1LL, 2LL, 3LL, 4LL});    // 55
    for (auto &&__item_21 : __range_21) {    // 55
        x = __item_21;    // 55
        if ((s.__contains__(x))) {    // 56
            count = (count + 1LL);    // 57
        }    // 56
    }    // 55
    print(tuple(count), str(" "), str("\n"));    // 58
    return 0LL;    // 60
}    // 1
}

namespace prog_print {

int run();    // 1

int run() {    // 1
    int64_t a;    // 2
    int64_t b;    // 3
    int64_t c;    // 4
    a = 1LL;    // 2
    b = 2LL;    // 3
    c = 3LL;    // 4
    print(tuple(), str(" "), str("\n"));    // 5
    print(tuple(a), str(" "), str("\n"));    // 6
    print(tuple(a, b, c), str(" "), str("\n"));    // 7
    print(tuple(), str(" "), str("end"));    // 8
    print(tuple(), str("sep"), str("\n"));    // 9
    print(tuple(a, b, c), str("-"), str("\n"));    // 10
    print(tuple(a), str(" "), str("()"));    // 11
    print(tuple(b), str(" "), str("()"));    // 12
    print(tuple(c), str(" "), str("\n"));    // 13
    print(tuple(a, b, c), str("sep"), str("end"));    // 15
    return 0LL;    // 17
}    // 1
}

namespace prog_set {

int run();    // 1

int run() {    // 1
    set<int64_t> s;    // 3
    set<int64_t> a;    // 20
    set<int64_t> b;    // 21
    set<int64_t> small;    // 32
    set<int64_t> big;    // 33
    set<int64_t> nine;    // 36
    set<int64_t> c;    // 44
    set<int64_t> d;    // 48
    set<int64_t> keep;    // 51
    set<int64_t> drop;    // 54
    int64_t total;    // 63
    int64_t x;    // 64
    set<tuple<int64_t, int64_t>> s1;    // 68
    set<tuple<int64_t, int64_t>> s2;    // 70
    set<str> s3;    // 72
    s = set<int64_t>({3LL, 1LL, 2LL, 1LL});    // 3
    print(tuple(len(s)), str(" "), str("\n"));    // 4
    print(tuple(sorted(s, false)), str(" "), str("\n"));    // 5
    print(tuple(sorted(s, true)), str(" "), str("\n"));    // 6
    print(tuple(sorted(s, [](auto x) { return (-x); }, false)), str(" "), str("\n"));    // 7
    print(tuple(sorted(s, [](auto x) { return (-x); }, true)), str(" "), str("\n"));    // 8
    s.add(4LL);    // 10
    print(tuple(sorted(s, false)), str(" "), str("\n"));    // 11
    s.add(4LL);    // 12
    print(tuple(len(s)), str(" "), str("\n"));    // 13
    s.remove(4LL);    // 15
    print(tuple(sorted(s, false)), str(" "), str("\n"));    // 16
    s.discard(99LL);    // 17
    print(tuple(sorted(s, false)), str(" "), str("\n"));    // 18
    a = set<int64_t>({1LL, 2LL, 3LL});    // 20
    b = set<int64_t>({3LL, 4LL});    // 21
    print(tuple(sorted((a | b), false)), str(" "), str("\n"));    // 22
    print(tuple(sorted((a & b), false)), str(" "), str("\n"));    // 23
    print(tuple(sorted((a - b), false)), str(" "), str("\n"));    // 24
    print(tuple(sorted((a ^ b), false)), str(" "), str("\n"));    // 25
    print(tuple(sorted(a.union_(b), false)), str(" "), str("\n"));    // 26
    print(tuple(sorted(a.intersection(b), false)), str(" "), str("\n"));    // 27
    print(tuple(sorted(a.difference(b), false)), str(" "), str("\n"));    // 28
    print(tuple(sorted(a.symmetric_difference(b), false)), str(" "), str("\n"));    // 29
    print(tuple(sorted(a, false), sorted(b, false)), str(" "), str("\n"));    // 30
    small = set<int64_t>({1LL, 2LL});    // 32
    big = set<int64_t>({1LL, 2LL, 3LL});    // 33
    print(tuple(small.issubset(big)), str(" "), str("\n"));    // 34
    print(tuple(big.issuperset(small)), str(" "), str("\n"));    // 35
    nine = set<int64_t>({9LL});    // 36
    print(tuple(small.isdisjoint(nine)), str(" "), str("\n"));    // 37
    print(tuple(small.isdisjoint(big)), str(" "), str("\n"));    // 38
    print(tuple(((small <= big)), ((small < big))), str(" "), str("\n"));    // 39
    print(tuple(((big >= small)), ((big > small))), str(" "), str("\n"));    // 40
    print(tuple(((set<int64_t>({1LL, 2LL}) == set<int64_t>({2LL, 1LL})))), str(" "), str("\n"));    // 41
    print(tuple(((small != big))), str(" "), str("\n"));    // 42
    c = a.copy();    // 44
    c.add(99LL);    // 45
    print(tuple(sorted(a, false), sorted(c, false)), str(" "), str("\n"));    // 46
    d = set<int64_t>({1LL, 2LL, 3LL});    // 48
    d.update(set<int64_t>({4LL}));    // 49
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 50
    keep = set<int64_t>({2LL, 3LL, 4LL});    // 51
    d.intersection_update(keep);    // 52
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 53
    drop = set<int64_t>({4LL});    // 54
    d.difference_update(drop);    // 55
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 56
    d.symmetric_difference_update(set<int64_t>({3LL, 5LL}));    // 57
    print(tuple(sorted(d, false)), str(" "), str("\n"));    // 58
    d.clear();    // 60
    print(tuple(len(d)), str(" "), str("\n"));    // 61
    total = 0LL;    // 63
    auto && __range_22 = set<int64_t>({1LL, 2LL, 3LL});    // 64
    for (auto &&__item_22 : __range_22) {    // 64
        x = __item_22;    // 64
        total = (total + x);    // 65
    }    // 64
    print(tuple(total), str(" "), str("\n"));    // 66
    s1 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 3LL)});    // 68
    print(tuple(sorted(s1, false)), str(" "), str("\n"));    // 69
    s2 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 2LL)});    // 70
    print(tuple(sorted(s2, false)), str(" "), str("\n"));    // 71
    s3 = set<str>({str("Hello"), str("World")});    // 72
    print(tuple(sorted(s3, false)), str(" "), str("\n"));    // 73
    return 0LL;    // 75
}    // 1
}

namespace prog_slice {

int run();    // 4

int run() {    // 4
    list<int64_t> l;    // 5
    list<int64_t> empty;    // 38
    list<int64_t> original;    // 44
    list<int64_t> copied;    // 45
    str s;    // 49
    l = list<int64_t>({0LL, 1LL, 2LL, 3LL, 4LL, 5LL});    // 5
    print(tuple(l[slice(0LL, 1LL, std::nullopt)]), str(" "), str("\n"));    // 7
    print(tuple(l[slice(1LL, 4LL, std::nullopt)]), str(" "), str("\n"));    // 8
    print(tuple(l[slice(std::nullopt, 3LL, std::nullopt)]), str(" "), str("\n"));    // 9
    print(tuple(l[slice(3LL, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 10
    print(tuple(l[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 11
    print(tuple(l[slice(std::nullopt, std::nullopt, 2LL)]), str(" "), str("\n"));    // 13
    print(tuple(l[slice(1LL, 5LL, 2LL)]), str(" "), str("\n"));    // 14
    print(tuple(l[slice(std::nullopt, std::nullopt, 3LL)]), str(" "), str("\n"));    // 15
    print(tuple(l[slice((-3LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 17
    print(tuple(l[slice(std::nullopt, (-2LL), std::nullopt)]), str(" "), str("\n"));    // 18
    print(tuple(l[slice((-4LL), (-1LL), std::nullopt)]), str(" "), str("\n"));    // 19
    print(tuple(l[slice((-1LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 20
    print(tuple(l[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));    // 22
    print(tuple(l[slice(4LL, 1LL, (-1LL))]), str(" "), str("\n"));    // 23
    print(tuple(l[slice(std::nullopt, std::nullopt, (-2LL))]), str(" "), str("\n"));    // 24
    print(tuple(l[slice((-1LL), (-4LL), (-1LL))]), str(" "), str("\n"));    // 25
    print(tuple(l[slice(10LL, 20LL, std::nullopt)]), str(" "), str("\n"));    // 28
    print(tuple(l[slice((-100LL), 100LL, std::nullopt)]), str(" "), str("\n"));    // 29
    print(tuple(l[slice(std::nullopt, 100LL, std::nullopt)]), str(" "), str("\n"));    // 30
    print(tuple(l[slice((-100LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 31
    print(tuple(l[slice(2LL, 2LL, std::nullopt)]), str(" "), str("\n"));    // 34
    print(tuple(l[slice(4LL, 1LL, std::nullopt)]), str(" "), str("\n"));    // 35
    print(tuple(l[slice(1LL, 4LL, (-1LL))]), str(" "), str("\n"));    // 36
    empty = list<int64_t>();    // 38
    print(tuple(empty[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 39
    print(tuple(empty[slice(0LL, 5LL, std::nullopt)]), str(" "), str("\n"));    // 40
    print(tuple(empty[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));    // 41
    original = list<int64_t>({1LL, 2LL, 3LL});    // 44
    copied = copy(original[slice(std::nullopt, std::nullopt, std::nullopt)]);    // 45
    copied.append(4LL);    // 46
    print(tuple(original, copied), str(" "), str("\n"));    // 47
    s = str("abcdef");    // 49
    print(tuple(s[slice(0LL, 1LL, std::nullopt)]), str(" "), str("\n"));    // 51
    print(tuple(s[slice(1LL, 4LL, std::nullopt)]), str(" "), str("\n"));    // 52
    print(tuple(s[slice(std::nullopt, 3LL, std::nullopt)]), str(" "), str("\n"));    // 53
    print(tuple(s[slice(3LL, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 54
    print(tuple(s[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 55
    print(tuple(s[slice(std::nullopt, std::nullopt, 2LL)]), str(" "), str("\n"));    // 57
    print(tuple(s[slice(1LL, 5LL, 2LL)]), str(" "), str("\n"));    // 58
    print(tuple(s[slice((-3LL), std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 60
    print(tuple(s[slice(std::nullopt, (-2LL), std::nullopt)]), str(" "), str("\n"));    // 61
    print(tuple(s[slice((-4LL), (-1LL), std::nullopt)]), str(" "), str("\n"));    // 62
    print(tuple(s[slice(std::nullopt, std::nullopt, (-1LL))]), str(" "), str("\n"));    // 64
    print(tuple(s[slice(4LL, 1LL, (-1LL))]), str(" "), str("\n"));    // 65
    print(tuple(s[slice(std::nullopt, std::nullopt, (-2LL))]), str(" "), str("\n"));    // 66
    print(tuple(s[slice(100LL, 200LL, std::nullopt)]), str(" "), str("\n"));    // 68
    print(tuple(s[slice((-100LL), 100LL, std::nullopt)]), str(" "), str("\n"));    // 69
    print(tuple(s[slice(2LL, 2LL, std::nullopt)]), str(" "), str("\n"));    // 70
    print(tuple(s[slice(4LL, 1LL, std::nullopt)]), str(" "), str("\n"));    // 71
    print(tuple(str("")[slice(std::nullopt, std::nullopt, std::nullopt)]), str(" "), str("\n"));    // 73
    print(tuple(str("")[slice(0LL, 5LL, std::nullopt)]), str(" "), str("\n"));    // 74
    return 0LL;    // 76
}    // 4
}

namespace prog_string {

int run();    // 1

int run() {    // 1
    str s;    // 2
    str padded;    // 30
    str a;    // 55
    str b;    // 56
    str joined;    // 68
    str c;    // 69
    s = str("Hello World");    // 2
    print(tuple(s), str(" "), str("\n"));    // 3
    print(tuple(len(s)), str(" "), str("\n"));    // 4
    print(tuple(s[0LL], s.back()), str(" "), str("\n"));    // 5
    print(tuple(s.upper()), str(" "), str("\n"));    // 7
    print(tuple(s.lower()), str(" "), str("\n"));    // 8
    print(tuple(s.swapcase()), str(" "), str("\n"));    // 9
    print(tuple(s.capitalize()), str(" "), str("\n"));    // 10
    print(tuple(str("hello world").title()), str(" "), str("\n"));    // 11
    print(tuple(s.casefold()), str(" "), str("\n"));    // 12
    print(tuple(s.find(str("o"))), str(" "), str("\n"));    // 14
    print(tuple(s.find(str("o"), 5LL)), str(" "), str("\n"));    // 15
    print(tuple(s.rfind(str("o"))), str(" "), str("\n"));    // 16
    print(tuple(s.find(str("zz"))), str(" "), str("\n"));    // 17
    print(tuple(s.index(str("World"))), str(" "), str("\n"));    // 18
    print(tuple(s.count(str("l"))), str(" "), str("\n"));    // 19
    print(tuple(s.count(str("zz"))), str(" "), str("\n"));    // 20
    print(tuple(s.startswith(str("Hello"))), str(" "), str("\n"));    // 21
    print(tuple(s.startswith(str("World"))), str(" "), str("\n"));    // 22
    print(tuple(s.endswith(str("World"))), str(" "), str("\n"));    // 23
    print(tuple(s.replace(str("l"), str("L"))), str(" "), str("\n"));    // 25
    print(tuple(s.replace(str("l"), str("L"), 2LL)), str(" "), str("\n"));    // 26
    print(tuple(s.removeprefix(str("Hello "))), str(" "), str("\n"));    // 27
    print(tuple(s.removesuffix(str(" World"))), str(" "), str("\n"));    // 28
    padded = str("  spaced  ");    // 30
    print(tuple(padded.strip()), str(" "), str("\n"));    // 31
    print(tuple(padded.lstrip()), str(" "), str("\n"));    // 32
    print(tuple(padded.rstrip()), str(" "), str("\n"));    // 33
    print(tuple(str("xxhixx").strip(str("x"))), str(" "), str("\n"));    // 34
    print(tuple(str("hi").ljust(5LL, str("."))), str(" "), str("\n"));    // 36
    print(tuple(str("hi").rjust(5LL, str("."))), str(" "), str("\n"));    // 37
    print(tuple(str("hi").center(6LL, str("."))), str(" "), str("\n"));    // 38
    print(tuple(str("42").zfill(5LL)), str(" "), str("\n"));    // 39
    print(tuple(str("-42").zfill(5LL)), str(" "), str("\n"));    // 40
    print(tuple(str("abc").isalpha(), str("a1").isalpha()), str(" "), str("\n"));    // 42
    print(tuple(str("123").isdigit(), str("12a").isdigit()), str(" "), str("\n"));    // 43
    print(tuple(str("a1").isalnum(), str("a-1").isalnum()), str(" "), str("\n"));    // 44
    print(tuple(str("  ").isspace(), str("a ").isspace()), str(" "), str("\n"));    // 45
    print(tuple(str("ABC").isupper(), str("Abc").isupper()), str(" "), str("\n"));    // 46
    print(tuple(str("abc").islower(), str("Abc").islower()), str(" "), str("\n"));    // 47
    print(tuple(s.split()), str(" "), str("\n"));    // 49
    print(tuple(str("a,b,c").split(str(","))), str(" "), str("\n"));    // 50
    print(tuple(str("a,,b").split(str(","))), str(" "), str("\n"));    // 51
    print(tuple(str("-").join(str("a,b,c").split(str(",")))), str(" "), str("\n"));    // 52
    print(tuple(str("one\ntwo").splitlines()), str(" "), str("\n"));    // 53
    a = str("foo");    // 55
    b = str("bar");    // 56
    print(tuple((a + b)), str(" "), str("\n"));    // 57
    print(tuple((a * 3LL)), str(" "), str("\n"));    // 58
    print(tuple(((a == str("foo"))), ((a == b))), str(" "), str("\n"));    // 59
    print(tuple(((a < b)), ((a > b))), str(" "), str("\n"));    // 60
    print(tuple(to_str(42LL)), str(" "), str("\n"));    // 62
    print(tuple(to_str(3.5)), str(" "), str("\n"));    // 63
    print(tuple(to_str(true)), str(" "), str("\n"));    // 64
    print(tuple(to_int(str("100"))), str(" "), str("\n"));    // 65
    print(tuple(to_float(str("0.5"))), str(" "), str("\n"));    // 66
    joined = str("");    // 68
    auto && __range_23 = str("abc");    // 69
    for (auto &&__item_23 : __range_23) {    // 69
        c = __item_23;    // 69
        joined = ((joined + c) + str("."));    // 70
    }    // 69
    print(tuple(joined), str(" "), str("\n"));    // 71
    return 0LL;    // 73
}    // 1
}

namespace prog_truthy {

int run();    // 1

int run() {    // 1
    int64_t a;    // 2
    int64_t b;    // 3
    str s1;    // 13
    str s2;    // 14
    list<int64_t> empty;    // 24
    list<int64_t> full;    // 25
    int64_t n;    // 44
    a = 0LL;    // 2
    b = 5LL;    // 3
    if (a) {    // 4
        print(tuple(str("a truthy")), str(" "), str("\n"));    // 5
    } else {    // 4
        print(tuple(str("a falsy")), str(" "), str("\n"));    // 7
    }    // 4
    if (b) {    // 8
        print(tuple(str("b truthy")), str(" "), str("\n"));    // 9
    } else {    // 8
        print(tuple(str("b falsy")), str(" "), str("\n"));    // 11
    }    // 8
    s1 = str("");    // 13
    s2 = str("hello");    // 14
    if (to_bool(s1)) {    // 15
        print(tuple(str("s1 truthy")), str(" "), str("\n"));    // 16
    } else {    // 15
        print(tuple(str("s1 falsy")), str(" "), str("\n"));    // 18
    }    // 15
    if (to_bool(s2)) {    // 19
        print(tuple(str("s2 truthy")), str(" "), str("\n"));    // 20
    } else {    // 19
        print(tuple(str("s2 falsy")), str(" "), str("\n"));    // 22
    }    // 19
    empty = list<int64_t>();    // 24
    full = list<int64_t>({1LL, 2LL, 3LL});    // 25
    if (to_bool(empty)) {    // 26
        print(tuple(str("empty truthy")), str(" "), str("\n"));    // 27
    } else {    // 26
        print(tuple(str("empty falsy")), str(" "), str("\n"));    // 29
    }    // 26
    if (to_bool(full)) {    // 30
        print(tuple(str("full truthy")), str(" "), str("\n"));    // 31
    } else {    // 30
        print(tuple(str("full falsy")), str(" "), str("\n"));    // 33
    }    // 30
    print(tuple((!to_bool(a))), str(" "), str("\n"));    // 35
    print(tuple((!to_bool(b))), str(" "), str("\n"));    // 36
    print(tuple(to_bool(a)), str(" "), str("\n"));    // 37
    print(tuple(to_bool(b)), str(" "), str("\n"));    // 38
    print(tuple(to_bool(0.0)), str(" "), str("\n"));    // 39
    print(tuple(to_bool(1.5)), str(" "), str("\n"));    // 40
    print(tuple(to_bool(true)), str(" "), str("\n"));    // 41
    print(tuple(to_bool(false)), str(" "), str("\n"));    // 42
    n = 3LL;    // 44
    while (n) {    // 45
        print(tuple(n), str(" "), str("\n"));    // 46
        n = (n - 1LL);    // 47
    }    // 45
    return 0LL;    // 49
}    // 1
}

namespace prog_tuple {

int run();    // 4

int run() {    // 4
    int64_t a;    // 6
    int64_t b;    // 6
    int64_t x;    // 14
    str s;    // 14
    int64_t p;    // 18
    int64_t q;    // 18
    int64_t m;    // 19
    int64_t n;    // 19
    tuple<int64_t, int64_t> t;    // 22
    tuple<int64_t, int64_t> t2;    // 23
    int64_t z;    // 25
    destructure(a, b) = tuple(1LL, 2LL);    // 6
    print(tuple(str("Test 1 - Simple destructure:"), a, b), str(" "), str("\n"));    // 7
    destructure(a, b) = tuple(10LL, 20LL);    // 10
    print(tuple(str("Test 2 - Reassign:"), a, b), str(" "), str("\n"));    // 11
    destructure(x, s) = tuple(42LL, str("hello"));    // 14
    print(tuple(str("Test 3 - Mixed types:"), x, s), str(" "), str("\n"));    // 15
    destructure(p, q) = tuple(100LL, 200LL);    // 18
    destructure(m, n) = tuple(p, q);    // 19
    print(tuple(str("Test 4 - Chained destructure:"), m, n), str(" "), str("\n"));    // 20
    t = tuple(1LL, 2LL);    // 22
    t2 = copy(t);    // 23
    z = t.get<0>();    // 25
    z = t.get<1>();    // 26
    return 0LL;    // 28
}    // 4
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

