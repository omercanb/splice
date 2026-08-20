#pragma once

// Python builtins that aren't methods on a container. Anything over an
// iterable goes through begin()/end(), the same protocol a for loop uses,
// so any of our containers - or a user class with begin()/end() - work here.

#include "mathops.h"
#include "str.h"
#include "truthy.h"
#include "tuple.h"
#include "types.h"
#include <cmath>
#include <string>
#include <type_traits>

namespace py {

template <typename Container> auto sum(Container &&c) {
    auto it = c.begin();
    auto end = c.end();
    // Value initialised, so an empty iterable sums to 0 as in Python.
    std::decay_t<decltype(*it)> total{};
    for (; it != end; ++it)
        total = total + *it;
    return total;
}

template <typename Container> bool any(Container &&c) {
    for (auto &&item : c)
        if (to_bool(item))
            return true;
    return false;
}

template <typename Container> bool all(Container &&c) {
    for (auto &&item : c)
        if (!to_bool(item))
            return false;
    return true;
}

template <typename Container> auto min(Container &&c) {
    auto it = c.begin();
    auto end = c.end();
    if (it == end)
        throw ValueError("min() iterable argument is empty");
    auto best = *it;
    for (++it; it != end; ++it)
        if (*it < best)
            best = *it;
    return best;
}

template <typename Container> auto max(Container &&c) {
    auto it = c.begin();
    auto end = c.end();
    if (it == end)
        throw ValueError("max() iterable argument is empty");
    auto best = *it;
    for (++it; it != end; ++it)
        if (best < *it)
            best = *it;
    return best;
}

// The two argument form. Ties go to the first, as in Python.
template <typename A, typename B> auto min(const A &a, const B &b) {
    return b < a ? b : a;
}

template <typename A, typename B> auto max(const A &a, const B &b) {
    return a < b ? b : a;
}

// idiv and mod already floor the way Python does, so the pair agrees with it.
inline tuple<_int, _int> divmod(_int a, _int b) {
    return tuple<_int, _int>(idiv(a, b), mod(a, b));
}

// Python rounds halves to even, which is the default floating point mode, so
// nearbyint gives round(2.5) == 2 and round(3.5) == 4 without extra work.
inline _int round(_float x) { return static_cast<_int>(std::nearbyint(x)); }
inline _int round(_int x) { return x; }

inline _float round(_float x, _int digits) {
    _float factor = std::pow(10.0, static_cast<_float>(digits));
    return std::nearbyint(x * factor) / factor;
}

inline str chr(_int code) { return str(std::string(1, static_cast<char>(code))); }

inline _int ord(const str &s) {
    if (s.__len__() != 1)
        throw TypeError("ord() expected a character");
    return static_cast<_int>(static_cast<unsigned char>(s.raw()[0]));
}

} // namespace py
