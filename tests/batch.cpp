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
    print(str("Test 1 - index/assign:"), a[0LL], a[1LL], a[2LL]);
    b = Array<uint32_t, 4>();
    print(str("Test 2 - Literal size:"), b[0LL], b[3LL]);
    print(str("Test 3 - last element:"), a.back());
    print(str("Test 4 - len:"), len(a), len(b));
    b.fill(uint32_t(1LL));
    print(str("Test 5 - fill:"), b[0LL], b[1LL], b[2LL], b[3LL]);
    total = 0LL;
    auto && __range_0 = b;
    for (auto &&__item_0 : __range_0) {
        v = __item_0;
        total = (total + v);
    }
    print(str("Test 6 - iteration sum:"), total);
    print(str("Test 7 - str:"), a);
    return 0LL;
}
}

namespace prog_boolops {

int64_t side(const int64_t &RESTRICT v);
int run();

int64_t side(const int64_t &RESTRICT v) {
    print(str("SIDE"));
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
    print(_and(a, b));
    print(_or(a, b));
    print(_and(zero, b));
    print(_or(zero, b));
    print(_and(zero, zero));
    print(_or(zero, zero));
    empty = str("");
    text = str("hi");
    print(_or(empty, text));
    print(_and(text, empty));
    print(_or(text, empty));
    print(_and(empty, text));
    no_items = list<int64_t>();
    print(_or(no_items, list<int64_t>({1LL, 2LL})));
    print(_and(list<int64_t>({3LL}), no_items));
    print(_or(list<int64_t>({3LL}), list<int64_t>({4LL})));
    print(_and(no_items, list<int64_t>({4LL})));
    f = 0.0;
    g = 2.5;
    print(_or(f, g));
    print(_and(g, f));
    print(_and(zero, side(9LL)));
    print(_or(a, side(9LL)));
    print(_and(a, side(9LL)));
    print(_or(zero, side(9LL)));
    print(_and(a, _and(b, 3LL)));
    print(_or(zero, _or(zero, 3LL)));
    print(_or(_and(a, b), 3LL));
    print(_or(zero, _and(b, 3LL)));
    print((_and(a, b) + 1LL));
    if ((a && to_bool(text))) {
        print(str("cond and"));
    }
    if ((to_bool(empty) || a)) {
        print(str("cond or"));
    }
    if ((!(to_bool(empty) || zero))) {
        print(str("cond not"));
    }
    if ((a && (to_bool(text) && b))) {
        print(str("cond chained"));
    }
    if ((to_bool(no_items) || to_bool(text))) {
        print(str("cond mixed"));
    }
    if ((zero && side(9LL))) {
        print(str("unreachable"));
    }
    n = 0LL;
    while ((((n < 2LL)) && to_bool(text))) {
        n = (n + 1LL);
    }
    print(n);
    print((!to_bool(a)));
    print((!to_bool(zero)));
    print((!to_bool(empty)));
    print((!to_bool(_and(a, b))));
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
    print(sum(numbers), sum(empty));
    print(min(numbers), max(numbers));
    print(min(3LL, 8LL), max(3LL, 8LL), min(2.5, 1.5));
    zeros = list<int64_t>({0LL, 0LL});
    with_zero = list<int64_t>({1LL, 0LL, 2LL});
    print(any(numbers), any(zeros), any(empty));
    print(all(numbers), all(with_zero), all(empty));
    print(sum(range(5LL)), max(range(5LL)));
    print(sum(set<int64_t>({1LL, 2LL, 3LL})));
    print(min(str("hello")), max(str("hello")));
    print(divmod(7LL, 2LL), divmod((-7LL), 2LL));
    print(py::round(2.5), py::round(3.5), py::round((-2.5)));
    print(py::round(2.567, 1LL), py::round(2.567, 2LL));
    print(py::round(5LL));
    print(chr(65LL), 65LL);
    print(chr((97LL + 1LL)));
    print(sum(__list_comprehension_0(numbers)));
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
    print(b);
    print(len(b));
    print(b[0LL], b.back());
    print(b.upper());
    print(b.lower());
    print(b.swapcase());
    print(b.capitalize());
    print(bytes(std::string("hello world", 11)).title());
    print(b.find(bytes(std::string("o", 1))));
    print(b.find(bytes(std::string("o", 1)), 5LL));
    print(b.rfind(bytes(std::string("o", 1))));
    print(b.find(bytes(std::string("zz", 2))));
    print(b.index(bytes(std::string("World", 5))));
    print(b.count(bytes(std::string("l", 1))));
    print(b.count(bytes(std::string("zz", 2))));
    print(b.startswith(bytes(std::string("Hello", 5))));
    print(b.startswith(bytes(std::string("World", 5))));
    print(b.endswith(bytes(std::string("World", 5))));
    print(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1))));
    print(b.replace(bytes(std::string("l", 1)), bytes(std::string("L", 1)), 2LL));
    print(b.removeprefix(bytes(std::string("Hello ", 6))));
    print(b.removesuffix(bytes(std::string(" World", 6))));
    padded = bytes(std::string("  spaced  ", 10));
    print(padded.strip());
    print(padded.lstrip());
    print(padded.rstrip());
    print(bytes(std::string("xxhixx", 6)).strip(bytes(std::string("x", 1))));
    print(bytes(std::string("hi", 2)).ljust(5LL, bytes(std::string(".", 1))));
    print(bytes(std::string("hi", 2)).rjust(5LL, bytes(std::string(".", 1))));
    print(bytes(std::string("hi", 2)).center(6LL, bytes(std::string(".", 1))));
    print(bytes(std::string("42", 2)).zfill(5LL));
    print(bytes(std::string("-42", 3)).zfill(5LL));
    print(bytes(std::string("abc", 3)).isalpha(), bytes(std::string("a1", 2)).isalpha());
    print(bytes(std::string("123", 3)).isdigit(), bytes(std::string("12a", 3)).isdigit());
    print(bytes(std::string("a1", 2)).isalnum(), bytes(std::string("a-1", 3)).isalnum());
    print(bytes(std::string("  ", 2)).isspace(), bytes(std::string("a ", 2)).isspace());
    print(bytes(std::string("ABC", 3)).isupper(), bytes(std::string("Abc", 3)).isupper());
    print(bytes(std::string("abc", 3)).islower(), bytes(std::string("Abc", 3)).islower());
    print(b.split());
    print(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1))));
    print(bytes(std::string("a,,b", 4)).split(bytes(std::string(",", 1))));
    print(bytes(std::string("-", 1)).join(bytes(std::string("a,b,c", 5)).split(bytes(std::string(",", 1)))));
    print(bytes(std::string("one\ntwo", 7)).splitlines());
    a = bytes(std::string("foo", 3));
    c = bytes(std::string("bar", 3));
    print((a + c));
    print((a * 3LL));
    print(((a == bytes(std::string("foo", 3)))), ((a == c)));
    print(((a < c)), ((a > c)));
    print((b.__contains__(bytes(std::string("World", 5)))));
    print((b.__contains__(87LL)));
    print((b.__contains__(90LL)));
    print(bytes(3LL));
    print(bytes(list<int64_t>({65LL, 66LL, 67LL})));
    total = 0LL;
    auto && __range_2 = bytes(std::string("abc", 3));
    for (auto &&__item_2 : __range_2) {
        byte = __item_2;
        total += byte;
    }
    print(total);
    joined = bytes(std::string("", 0));
    auto && __range_3 = bytes(std::string("abc", 3));
    for (auto &&__item_3 : __range_3) {
        byte = __item_3;
        joined = ((joined + bytes(list<int64_t>({byte}))) + bytes(std::string(".", 1)));
    }
    print(joined);
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
    print(a, b, c, f1, i1, i2);
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
    print(counter.count, counter.doubled());
    p = Point(3LL, 4LL);
    print(p.x, p.y, p.norm());
    print(p);
    print(len(p));
    print(to_bool(p), to_bool(Point(0LL, 0LL)));
    q = p.moved(1LL, 1LL);
    print(q, q.norm());
    p.x = 10LL;
    print(p.x, p.norm());
    points = list<Point>({Point(1LL, 1LL), Point(2LL, 2LL)});
    auto && __range_4 = points;
    for (auto &&__item_4 : __range_4) {
        point = __item_4;
        print(point, point.norm());
    }
    print(len(points));
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
    print(((a < b) && (b < c)));
    print(((a > b)));
    print(((a > b) && (b > c)));
    d = 3LL;
    print(((c <= d)));
    print(((a == d)));
    print(((c == d)));
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
    print(__list_comprehension_2(numbers));
    print(__list_comprehension_3(numbers));
    print(__list_comprehension_4());
    print(__list_comprehension_5());
    print(__list_comprehension_6(numbers));
    print(scaled(numbers, 10LL));
    squares = __set_comprehension_0(numbers);
    print(sorted(squares));
    lookup = __dict_comprehension_0(numbers);
    print(len(lookup), lookup[2LL], lookup[4LL]);
    print(sorted(lookup));
    print(__list_comprehension_7(numbers));
    print(__list_comprehension_8(numbers));
    doubled = __list_comprehension_9(numbers);
    print(__list_comprehension_10(doubled));
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
    print(len(d));
    print(d[1LL], d[2LL], d[3LL]);
    print(sorted(d));
    print(_sorted_kwargs(true, d));
    d[4LL] = 40LL;
    print(len(d), d[4LL]);
    d[1LL] = 11LL;
    print(len(d), d[1LL]);
    print(d.get(1LL));
    print(d.get(99LL, (-1LL)));
    print(d.pop(4LL));
    print(d.pop(99LL, (-1LL)));
    print(len(d));
    print(d.setdefault(2LL, 999LL));
    print(d.setdefault(9LL, 90LL));
    print(sorted(d));
    print(sorted(d.keys()));
    print(sorted(d.values()));
    e = dict<int64_t, int64_t>({{5LL, 50LL}});
    d.update(e);
    print(sorted(d));
    c = d.copy();
    print(len(c));
    c.clear();
    print(len(c), len(d));
    s = dict<str, int64_t>({{str("b"), 2LL}, {str("a"), 1LL}});
    print(sorted(s));
    print(s[str("a")], s[str("b")]);
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
            print(str("cleanup"), text);
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
            print(str("bad literal"), text);
            return fallback;
        }
        if (!__thrown) {
            print(str("good literal"), text);
            return value;
        }
    }
}

int64_t nested() {
    {
        Finally __finally([&] {
            print(str("outer finally"));
        });
        try {
            {
                Finally __finally([&] {
                    print(str("inner finally"));
                });
                return to_int(str("nope"));
            }
        } catch (ValueError &) {
            print(str("outer caught"));
            return (-1LL);
        }
    }
}

int64_t handler_raises() {
    try {
        {
            Finally __finally([&] {
                print(str("guard ran"));
            });
            bool __thrown = false;
            try {
                print(to_int(str("bad")));
            } catch (ValueError &) {
                __thrown = true;
                print(str("handler raising"));
                return to_int(str("worse"));
            }
            if (!__thrown) {
                print(str("not reached"));
            }
        }
    } catch (ValueError &) {
        print(str("caught the handler's exception"));
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
            print(str("reraise finally"));
        });
        try {
            return check_positive(n);
        } catch (ValueError &) {
            print(str("logging and passing it on"));
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
            print(to_int(first));
        } catch (ValueError &) {
            __thrown = true;
            print(str("first was bad"));
        }
        if (!__thrown) {
            print(str("first was fine"));
        }
    }
    {
        bool __thrown = false;
        try {
            print(to_int(second));
        } catch (ValueError &) {
            __thrown = true;
            print(str("second was bad"));
        }
        if (!__thrown) {
            print(str("second was fine"));
        }
    }
    return 0LL;
}

int64_t relay() {
    try {
        throw ValueError(str("original"));
    } catch (PyException &e) {
        print(str("relaying"));
        e.raise();
    }
}

int run() {
    list<int64_t> numbers;
    list<int64_t> items;
    int64_t i;
    print(guarded_parse(str("41")));
    try {
        print(guarded_parse(str("zzz")));
    } catch (ValueError &) {
        print(str("caught from callee"));
    }
    print(parse_or(str("7"), 0LL));
    print(parse_or(str("seven"), 0LL));
    print(nested());
    print(handler_raises());
    print(check_positive(3LL));
    try {
        print(check_positive((-1LL)));
    } catch (ValueError &) {
        print(str("caught the raise"));
    }
    try {
        print(reraise((-2LL)));
    } catch (ValueError &) {
        print(str("caught the re-raise"));
    }
    try {
        print(raise_bare_class(str("k")));
    } catch (KeyError &) {
        print(str("caught the bare class"));
    }
    print(siblings(str("8"), str("eight")));
    try {
        print(relay());
    } catch (ValueError &) {
        print(str("still a ValueError after the relay"));
    }
    try {
        throw TypeError(str("wrong type"));
    } catch (PyException &) {
        print(str("base handler took the subclass"));
    }
    numbers = list<int64_t>({1LL, 2LL, 3LL});
    try {
        print(numbers[10LL]);
    } catch (IndexError &) {
        print(str("index error wins over the base class"));
    } catch (PyException &) {
        print(str("not reached"));
    }
    items = list<int64_t>({1LL, 2LL, 3LL});
    try {
        print(items[10LL]);
    } catch (PyException &) {
        print(str("bare except caught it"));
    }
    try {
        print(to_float(str("x")));
    } catch (ValueError &e) {
        print(str("float refused it"));
    }
    int64_t __stop_3 = 3LL;
    for (i = 0; i < __stop_3; ++i) {
        {
            Finally __finally([&] {
                print(str("loop finally"), i);
            });
            if (((i == 1LL))) {
                break;
            }
            print(str("loop"), i);
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
    print(len(text));
    print(text.splitlines());
    print(len(handle.read()));
    handle.close();
    stepped = open(SAMPLE);
    print(stepped.readline().strip());
    print(stepped.readline().strip());
    rest = stepped.readlines();
    print(len(rest), rest[0LL].strip());
    print(len(stepped.readlines()));
    lines = open(SAMPLE);
    auto && __range_15 = lines;
    for (auto &&__item_15 : __range_15) {
        line = __item_15;
        print(len(line), line.strip());
    }
    out = open(str("tests/test_files/sample_out.txt"), str("w"));
    print(out.write(str("alpha\n")));
    print(out.write(str("beta\n")));
    out.close();
    written = open(str("tests/test_files/sample_out.txt")).read();
    print(written.splitlines(), len(written));
    try {
        open(str("tests/test_files/no_such_file.txt"));
    } catch (FileNotFoundError &) {
        print(str("missing file raised"));
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
    print(n, m);
    doubled = scale(n, 2LL);
    print(doubled);
    total = (scale(n, 2LL) + n);
    print(total);
    print(clamp(500LL, 0LL, 100LL));
    print(clamp((-5LL), 0LL, 100LL));
    print(clamp(50LL, 0LL, 100LL));
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
    print(a, b);
    c = uint16_t(1000LL);
    d = add_u16(c, uint16_t(234LL));
    print(d);
    e = int32_t((-70000LL));
    f = uint32_t(70000LL);
    print(e, f);
    g = int64_t((-5000000000LL));
    h = uint64_t(5000000000LL);
    print(g, h);
    print(((uint16_t(5LL) == uint16_t(5LL))));
    print(((uint16_t(5LL) < uint16_t(10LL))));
    values = list<uint16_t>({uint16_t(3LL), uint16_t(1LL), uint16_t(2LL)});
    print(values);
    counts = dict<uint8_t, str>();
    counts[uint8_t(1LL)] = str("one");
    counts[uint8_t(2LL)] = str("two");
    print(counts[uint8_t(1LL)], counts[uint8_t(2LL)]);
    seen = set<int32_t>({int32_t(1LL), int32_t(2LL), int32_t(2LL)});
    print(len(seen));
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
    print(str("Test 1 - derived global:"), totalSlots);
    arr = Array<uint8_t, 24>();
    print(str("Test 2 - array sized by derived global:"), len(arr));
    print(str("Test 3 - shift masks:"), pair0_mask, pair2_mask);
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
    print(str("Test 1 - process_order:"), process_order(10007LL, 10023LL, 5LL, 5LL));
    print(str("Test 1b - process_order (sell):"), process_order(9998LL, 10001LL, (-3LL), 5LL));
    print(str("Test 2 - distance_squared:"), distance_squared(0LL, 0LL, 3LL, 4LL));
    print(str("Test 3 - sum_up_to:"), sum_up_to(5LL));
    v = Vector(3LL, 4LL);
    print(str("Test 4 - length_squared:"), v.length_squared());
    v.move(200LL, (-200LL));
    print(str("Test 5 - move (clamped):"), v.x, v.y);
    scaled = v.scaled(2LL);
    print(str("Test 6 - scaled:"), scaled.x, scaled.y);
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
    print(a);
    auto && __range_16 = map([](auto x) { return to_str(x); }, nums);
    for (auto &&__item_16 : __range_16) {
        s = __item_16;
        print(s);
    }
    filtered = list<int64_t>(filter([](auto x) { return ((mod(x, 2LL) == 0LL)); }, nums));
    print(filtered);
    auto && __range_17 = zip(nums, a);
    for (auto &&__item_17 : __range_17) {
        destructure(x, y) = __item_17;
        print(x, y);
    }
    nums = copy(a);
    auto && __range_18 = enumerate(nums);
    for (auto &&__item_18 : __range_18) {
        destructure(i, n) = __item_18;
        print(i, n);
    }
    return 0LL;
}
}

namespace prog_list {

list<int64_t> give_list(list<int64_t> &RESTRICT l);
int run();

list<int64_t> give_list(list<int64_t> &RESTRICT l) {
    print(l);
    l.append(2LL);
    print(l);
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
    print(list<int64_t>(list<int64_t>({1LL, 2LL, 3LL})));
    l = list<int64_t>({1LL, 2LL, 3LL});
    print(l);
    l = list<int64_t>({1LL, 2LL, 3LL});
    print(l);
    l.append(4LL);
    print(l);
    l = give_list(l);
    print(l);
    print(l[1LL]);
    a = l[0LL];
    print(l);
    l[0LL] = a;
    print(l);
    l[0LL] = 2LL;
    print(l);
    l2 = copy(l[slice(0LL, 1LL, std::nullopt)]);
    print(l2);
    l.insert(0LL, 100LL);
    print(l);
    l.insert(2LL, 200LL);
    print(l);
    l.insert((-1LL), 300LL);
    print(l);
    l.insert(100LL, 400LL);
    print(l);
    l.insert((-100LL), 500LL);
    print(l);
    l.remove(200LL);
    print(l);
    x = l.pop();
    print(x, l);
    y = l.pop(0LL);
    print(y, l);
    z = l.pop((-2LL));
    print(z, l);
    l.extend(list<int64_t>({7LL, 8LL}));
    print(l);
    l3 = l.copy();
    print(l3);
    l.clear();
    print(l);
    l2 = list<int64_t>({5LL, 3LL, 1LL, 3LL, 9LL});
    print(l2.index(3LL));
    print(l2.index(3LL, 3LL));
    print(l2.index(3LL, 0LL, 2LL));
    print(l2.index(9LL, (-2LL)));
    print(l2.count(3LL));
    print(l2.count(42LL));
    l2.sort();
    print(l2);
    l2.sort(true);
    print(l2);
    l2.sort(false);
    print(l2);
    l2.reverse();
    print(l2);
    n = len(l2);
    print(n);
    print(l2[0LL], l2.back());
    if (!(((l2.back() == l2[(n - 1LL)])))) throw AssertionError("");
    l4 = list<int64_t>({1LL, 2LL, 3LL});
    print(l4.back());
    l4.back() += 10LL;
    if (!(((l4.back() == 13LL)))) throw AssertionError("");
    print(l4);
    l5 = (list<int64_t>({1LL, 2LL}) * 3LL);
    print(l5);
    l5 = (3LL * list<int64_t>({1LL, 2LL}));
    print(l5);
    if (!(((l5 == (list<int64_t>({1LL, 2LL}) * 3LL))))) throw AssertionError("");
    l5 *= 2LL;
    print(l5);
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
        print(l);
    }
    int64_t __stop_5 = x;
    for (i = 0; i < __stop_5; ++i) {
        print(str("first"), i);
    }
    int64_t __stop_6 = (x + 5LL);
    for (i = x; i < __stop_6; ++i) {
        print(str("second"), i);
    }
    int64_t __stop_7 = (x + 10LL);
    for (i = x; i < __stop_7; i += 2) {
        print(str("third"), i);
    }
    int64_t __stop_8 = (x - 7LL);
    for (i = x; i > __stop_8; i += -2) {
        print(str("fourth"), i);
    }
    step = x;
    int64_t __stop_9 = (10LL * x);
    int64_t __step_0 = step;
    for (i = x;; i += __step_0) {
        if ((__step_0 > 0 && i >= __stop_9) || (__step_0 < 0 && i <= __stop_9)) break;
        print(str("fifth"), i);
    }
    step = (-2LL);
    int64_t __stop_10 = (10LL * x);
    int64_t __step_1 = step;
    for (i = (5LL * x);; i += __step_1) {
        if ((__step_1 > 0 && i >= __stop_10) || (__step_1 < 0 && i <= __stop_10)) break;
        print(str("sixth"), i);
    }
    auto && __range_19 = l;
    for (auto &&__item_19 : __range_19) {
        n = __item_19;
        print(str("seventh"), n);
    }
    auto && __range_20 = l;
    for (auto &&__item_20 : __range_20) {
        n = __item_20;
        print(str("eight"), n);
    }
    return 0LL;
}
}

namespace prog_math {

int run();

int run() {
    int64_t a;
    print(pow(10LL, 10LL));
    print(idiv((-10LL), 3LL));
    print(idiv(10LL, 3LL));
    print(fdiv(5LL, 2LL));
    print(pow(0.5, 4LL));
    print((50.0 * 100LL));
    a = pow(10LL, 10LL);
    print((-(-5LL)));
    print((~5LL));
    print((-(+5LL)));
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
    print((l.__contains__(2LL)), (l.__contains__(9LL)));
    print((!l.__contains__(2LL)), (!l.__contains__(9LL)));
    d = dict<int64_t, int64_t>({{1LL, 10LL}, {2LL, 20LL}});
    print((d.__contains__(1LL)), (d.__contains__(9LL)));
    print((d.values().__contains__(10LL)));
    print((!d.__contains__(1LL)), (!d.__contains__(9LL)));
    s = set<int64_t>({1LL, 2LL, 3LL});
    print((s.__contains__(2LL)), (s.__contains__(9LL)));
    print((!s.__contains__(2LL)), (!s.__contains__(9LL)));
    text = str("hello world");
    print((text.__contains__(str("hello"))), (text.__contains__(str("zz"))));
    print((text.__contains__(str("o w"))));
    print((!text.__contains__(str("hello"))), (!text.__contains__(str("zz"))));
    t = tuple(1LL, 2LL, 3LL);
    print((t.__contains__(2LL)), (t.__contains__(9LL)));
    pairs = list<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});
    print((pairs.__contains__(tuple(1LL, 2LL))), (pairs.__contains__(tuple(9LL, 9LL))));
    print((!pairs.__contains__(tuple(1LL, 2LL))), (!pairs.__contains__(tuple(9LL, 9LL))));
    print((pairs.__contains__(tuple(2LL, 1LL))));
    pair_set = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(3LL, 4LL)});
    print((pair_set.__contains__(tuple(1LL, 2LL))), (pair_set.__contains__(tuple(9LL, 9LL))));
    print((pair_set.__contains__(tuple(2LL, 1LL))));
    pair_dict = dict<tuple<int64_t, int64_t>, str>({{tuple(1LL, 2LL), str("a")}, {tuple(3LL, 4LL), str("b")}});
    print((pair_dict.__contains__(tuple(1LL, 2LL))), (pair_dict.__contains__(tuple(9LL, 9LL))));
    nested = tuple(tuple(1LL, 2LL), tuple(3LL, 4LL));
    print((nested.__contains__(tuple(1LL, 2LL))), (nested.__contains__(tuple(9LL, 9LL))));
    mixed = list<tuple<str, int64_t>>({tuple(str("a"), 1LL), tuple(str("b"), 2LL)});
    print((mixed.__contains__(tuple(str("a"), 1LL))), (mixed.__contains__(tuple(str("a"), 2LL))));
    strs = list<str>({str("a"), str("b")});
    print((strs.__contains__(str("a"))), (strs.__contains__(str("z"))));
    if ((l.__contains__(2LL))) {
        print(str("found"));
    } else {
        print(str("missing"));
    }
    count = 0LL;
    auto && __range_21 = list<int64_t>({1LL, 2LL, 3LL, 4LL});
    for (auto &&__item_21 : __range_21) {
        x = __item_21;
        if ((s.__contains__(x))) {
            count = (count + 1LL);
        }
    }
    print(count);
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
    print();
    print(a);
    print(a, b, c);
    _print_kwargs(str(" "), str("end"));
    _print_kwargs(str("sep"), str("\n"));
    _print_kwargs(str("-"), str("\n"), a, b, c);
    _print_kwargs(str(" "), str("()"), a);
    _print_kwargs(str(" "), str("()"), b);
    print(c);
    _print_kwargs(str("sep"), str("end"), a, b, c);
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
    print(len(s));
    print(sorted(s));
    print(_sorted_kwargs(true, s));
    s.add(4LL);
    print(sorted(s));
    s.add(4LL);
    print(len(s));
    s.remove(4LL);
    print(sorted(s));
    s.discard(99LL);
    print(sorted(s));
    a = set<int64_t>({1LL, 2LL, 3LL});
    b = set<int64_t>({3LL, 4LL});
    print(sorted((a | b)));
    print(sorted((a & b)));
    print(sorted((a - b)));
    print(sorted((a ^ b)));
    print(sorted(a.union_(b)));
    print(sorted(a.intersection(b)));
    print(sorted(a.difference(b)));
    print(sorted(a.symmetric_difference(b)));
    print(sorted(a), sorted(b));
    small = set<int64_t>({1LL, 2LL});
    big = set<int64_t>({1LL, 2LL, 3LL});
    print(small.issubset(big));
    print(big.issuperset(small));
    nine = set<int64_t>({9LL});
    print(small.isdisjoint(nine));
    print(small.isdisjoint(big));
    print(((small <= big)), ((small < big)));
    print(((big >= small)), ((big > small)));
    print(((set<int64_t>({1LL, 2LL}) == set<int64_t>({2LL, 1LL}))));
    print(((small != big)));
    c = a.copy();
    c.add(99LL);
    print(sorted(a), sorted(c));
    d = set<int64_t>({1LL, 2LL, 3LL});
    d.update(set<int64_t>({4LL}));
    print(sorted(d));
    keep = set<int64_t>({2LL, 3LL, 4LL});
    d.intersection_update(keep);
    print(sorted(d));
    drop = set<int64_t>({4LL});
    d.difference_update(drop);
    print(sorted(d));
    d.symmetric_difference_update(set<int64_t>({3LL, 5LL}));
    print(sorted(d));
    d.clear();
    print(len(d));
    total = 0LL;
    auto && __range_22 = set<int64_t>({1LL, 2LL, 3LL});
    for (auto &&__item_22 : __range_22) {
        x = __item_22;
        total = (total + x);
    }
    print(total);
    s1 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 3LL)});
    print(sorted(s1));
    s2 = set<tuple<int64_t, int64_t>>({tuple(1LL, 2LL), tuple(1LL, 2LL)});
    print(sorted(s2));
    s3 = set<str>({str("Hello"), str("World")});
    print(sorted(s3));
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
    print(l[slice(0LL, 1LL, std::nullopt)]);
    print(l[slice(1LL, 4LL, std::nullopt)]);
    print(l[slice(std::nullopt, 3LL, std::nullopt)]);
    print(l[slice(3LL, std::nullopt, std::nullopt)]);
    print(l[slice(std::nullopt, std::nullopt, std::nullopt)]);
    print(l[slice(std::nullopt, std::nullopt, 2LL)]);
    print(l[slice(1LL, 5LL, 2LL)]);
    print(l[slice(std::nullopt, std::nullopt, 3LL)]);
    print(l[slice((-3LL), std::nullopt, std::nullopt)]);
    print(l[slice(std::nullopt, (-2LL), std::nullopt)]);
    print(l[slice((-4LL), (-1LL), std::nullopt)]);
    print(l[slice((-1LL), std::nullopt, std::nullopt)]);
    print(l[slice(std::nullopt, std::nullopt, (-1LL))]);
    print(l[slice(4LL, 1LL, (-1LL))]);
    print(l[slice(std::nullopt, std::nullopt, (-2LL))]);
    print(l[slice((-1LL), (-4LL), (-1LL))]);
    print(l[slice(10LL, 20LL, std::nullopt)]);
    print(l[slice((-100LL), 100LL, std::nullopt)]);
    print(l[slice(std::nullopt, 100LL, std::nullopt)]);
    print(l[slice((-100LL), std::nullopt, std::nullopt)]);
    print(l[slice(2LL, 2LL, std::nullopt)]);
    print(l[slice(4LL, 1LL, std::nullopt)]);
    print(l[slice(1LL, 4LL, (-1LL))]);
    empty = list<int64_t>();
    print(empty[slice(std::nullopt, std::nullopt, std::nullopt)]);
    print(empty[slice(0LL, 5LL, std::nullopt)]);
    print(empty[slice(std::nullopt, std::nullopt, (-1LL))]);
    original = list<int64_t>({1LL, 2LL, 3LL});
    copied = copy(original[slice(std::nullopt, std::nullopt, std::nullopt)]);
    copied.append(4LL);
    print(original, copied);
    s = str("abcdef");
    print(s[slice(0LL, 1LL, std::nullopt)]);
    print(s[slice(1LL, 4LL, std::nullopt)]);
    print(s[slice(std::nullopt, 3LL, std::nullopt)]);
    print(s[slice(3LL, std::nullopt, std::nullopt)]);
    print(s[slice(std::nullopt, std::nullopt, std::nullopt)]);
    print(s[slice(std::nullopt, std::nullopt, 2LL)]);
    print(s[slice(1LL, 5LL, 2LL)]);
    print(s[slice((-3LL), std::nullopt, std::nullopt)]);
    print(s[slice(std::nullopt, (-2LL), std::nullopt)]);
    print(s[slice((-4LL), (-1LL), std::nullopt)]);
    print(s[slice(std::nullopt, std::nullopt, (-1LL))]);
    print(s[slice(4LL, 1LL, (-1LL))]);
    print(s[slice(std::nullopt, std::nullopt, (-2LL))]);
    print(s[slice(100LL, 200LL, std::nullopt)]);
    print(s[slice((-100LL), 100LL, std::nullopt)]);
    print(s[slice(2LL, 2LL, std::nullopt)]);
    print(s[slice(4LL, 1LL, std::nullopt)]);
    print(str("")[slice(std::nullopt, std::nullopt, std::nullopt)]);
    print(str("")[slice(0LL, 5LL, std::nullopt)]);
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
    print(s);
    print(len(s));
    print(s[0LL], s.back());
    print(s.upper());
    print(s.lower());
    print(s.swapcase());
    print(s.capitalize());
    print(str("hello world").title());
    print(s.casefold());
    print(s.find(str("o")));
    print(s.find(str("o"), 5LL));
    print(s.rfind(str("o")));
    print(s.find(str("zz")));
    print(s.index(str("World")));
    print(s.count(str("l")));
    print(s.count(str("zz")));
    print(s.startswith(str("Hello")));
    print(s.startswith(str("World")));
    print(s.endswith(str("World")));
    print(s.replace(str("l"), str("L")));
    print(s.replace(str("l"), str("L"), 2LL));
    print(s.removeprefix(str("Hello ")));
    print(s.removesuffix(str(" World")));
    padded = str("  spaced  ");
    print(padded.strip());
    print(padded.lstrip());
    print(padded.rstrip());
    print(str("xxhixx").strip(str("x")));
    print(str("hi").ljust(5LL, str(".")));
    print(str("hi").rjust(5LL, str(".")));
    print(str("hi").center(6LL, str(".")));
    print(str("42").zfill(5LL));
    print(str("-42").zfill(5LL));
    print(str("abc").isalpha(), str("a1").isalpha());
    print(str("123").isdigit(), str("12a").isdigit());
    print(str("a1").isalnum(), str("a-1").isalnum());
    print(str("  ").isspace(), str("a ").isspace());
    print(str("ABC").isupper(), str("Abc").isupper());
    print(str("abc").islower(), str("Abc").islower());
    print(s.split());
    print(str("a,b,c").split(str(",")));
    print(str("a,,b").split(str(",")));
    print(str("-").join(str("a,b,c").split(str(","))));
    print(str("one\ntwo").splitlines());
    a = str("foo");
    b = str("bar");
    print((a + b));
    print((a * 3LL));
    print(((a == str("foo"))), ((a == b)));
    print(((a < b)), ((a > b)));
    print(to_str(42LL));
    print(to_str(3.5));
    print(to_str(true));
    print(to_int(str("100")));
    print(to_float(str("0.5")));
    joined = str("");
    auto && __range_23 = str("abc");
    for (auto &&__item_23 : __range_23) {
        c = __item_23;
        joined = ((joined + c) + str("."));
    }
    print(joined);
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
        print(str("a truthy"));
    } else {
        print(str("a falsy"));
    }
    if (b) {
        print(str("b truthy"));
    } else {
        print(str("b falsy"));
    }
    s1 = str("");
    s2 = str("hello");
    if (to_bool(s1)) {
        print(str("s1 truthy"));
    } else {
        print(str("s1 falsy"));
    }
    if (to_bool(s2)) {
        print(str("s2 truthy"));
    } else {
        print(str("s2 falsy"));
    }
    empty = list<int64_t>();
    full = list<int64_t>({1LL, 2LL, 3LL});
    if (to_bool(empty)) {
        print(str("empty truthy"));
    } else {
        print(str("empty falsy"));
    }
    if (to_bool(full)) {
        print(str("full truthy"));
    } else {
        print(str("full falsy"));
    }
    print((!to_bool(a)));
    print((!to_bool(b)));
    print(to_bool(a));
    print(to_bool(b));
    print(to_bool(0.0));
    print(to_bool(1.5));
    print(to_bool(true));
    print(to_bool(false));
    n = 3LL;
    while (n) {
        print(n);
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
    print(str("Test 1 - Simple destructure:"), a, b);
    destructure(a, b) = tuple(10LL, 20LL);
    print(str("Test 2 - Reassign:"), a, b);
    destructure(x, s) = tuple(42LL, str("hello"));
    print(str("Test 3 - Mixed types:"), x, s);
    destructure(p, q) = tuple(100LL, 200LL);
    destructure(m, n) = tuple(p, q);
    print(str("Test 4 - Chained destructure:"), m, n);
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

