#pragma once

#include "types.h"
#include <cmath>

namespace py {

ALWAYS_INLINE _float pow(_float base, _int exp) { return std::pow(base, exp); }
ALWAYS_INLINE _float pow(_float base, _float exp) { return std::pow(base, exp); }
ALWAYS_INLINE _int pow(_int base, _int exp) {
    return static_cast<_int>(pow(static_cast<_float>(base), exp));
}

// Python and C++ modulo differ slightly for negative numerators
ALWAYS_INLINE _int mod(_int a, _int b) {
    _int r = a % b;
    return (r != 0 && (r ^ b) < 0) ? r + b : r;
}

ALWAYS_INLINE _float fdiv(_float a, _float b) { return a / b; }
ALWAYS_INLINE _float fdiv(_int a, _float b) { return static_cast<_float>(a) / b; }
ALWAYS_INLINE _float fdiv(_float a, _int b) { return a / static_cast<_float>(b); }
ALWAYS_INLINE _float fdiv(_int a, _int b) { return static_cast<_float>(a) / b; }
// Pythons '//' division that rounds to negative infinity
ALWAYS_INLINE _int idiv(_int a, _int b) { return a >= 0 ? a / b : -1 - (-1 - a) / b; }

} // namespace py
