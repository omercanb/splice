#pragma once

#include "exceptions.h"
#include "str.h"
#include "types.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>
#include <type_traits>

ALWAYS_INLINE _int to_int(_int v) { return v; }
ALWAYS_INLINE _int to_int(_float v) { return static_cast<_int>(v); }
ALWAYS_INLINE _int to_int(bool v) { return v ? 1 : 0; }

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
ALWAYS_INLINE _int to_int(T v) {
    return static_cast<_int>(v);
}

inline _int to_int(const py::str &s, _int base = 10) {
    std::string t = s.raw();
    t.erase(0, t.find_first_not_of(" \t\n"));
    t.erase(t.find_last_not_of(" \t\n") + 1);
    size_t pos;
    _int result;
    try {
        result = std::stoll(t, &pos, base);
    } catch (const std::logic_error &) {
        // invalid_argument when t holds no digits, out_of_range past _int
        throw py::ValueError("invalid literal for int(): " + s.raw());
    }
    if (pos != t.size())
        throw py::ValueError("invalid literal for int(): " + s.raw());
    return result;
}

ALWAYS_INLINE _float to_float(_int v) { return v; }
ALWAYS_INLINE _float to_float(_float v) { return v; }
ALWAYS_INLINE _float to_float(bool v) { return v ? 1.0 : 0.0; }

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
ALWAYS_INLINE _float to_float(T v) {
    return static_cast<_float>(v);
}

inline _float to_float(const py::str &s) {
    std::string t = s.raw();
    t.erase(0, t.find_first_not_of(" \t\n"));
    t.erase(t.find_last_not_of(" \t\n") + 1);

    std::string low = t;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    if (low == "inf" || low == "infinity")
        return INFINITY;
    if (low == "-inf" || low == "-infinity")
        return -INFINITY;
    if (low == "nan" || low == "-nan")
        return NAN;

    size_t pos;
    _float result;
    try {
        result = std::stod(t, &pos); // handles scientific notation
    } catch (const std::logic_error &) {
        throw py::ValueError("could not convert string to float: " + s.raw());
    }
    if (pos != t.size())
        throw py::ValueError("could not convert string to float: " + s.raw());
    return result;
}
