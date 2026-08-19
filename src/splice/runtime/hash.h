#pragma once

#include "types.h"
#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

namespace py {

// Required by dict keys and set elements: objects comparing equal must
// hash equal.

inline size_t hash(_int x) { return std::hash<_int>{}(x); }
inline size_t hash(_float x) { return std::hash<_float>{}(x); }
inline size_t hash(bool x) { return std::hash<bool>{}(x); }
inline size_t hash(const std::string &s) { return std::hash<std::string>{}(s); }

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
inline size_t hash(T x) {
    return hash(static_cast<_int>(x));
}

inline size_t hash_combine(size_t seed, size_t h) {
    return seed ^ (h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}

namespace detail {

template <typename T, typename = void>
struct has_hash_method : std::false_type {};
template <typename T>
struct has_hash_method<
    T, std::void_t<decltype(std::declval<const T &>().__hash__())>>
    : std::true_type {};

} // namespace detail

// Fallback for user-defined classes: prefer __hash__(), like CPython.
template <typename T, typename = std::enable_if_t<!std::is_integral_v<T>>>
inline size_t hash(const T &x) {
    static_assert(detail::has_hash_method<T>::value,
                  "unhashable type: needs a __hash__() method to be used as a "
                  "dict key or set element");
    return static_cast<size_t>(x.__hash__());
}

// Functor form for std::unordered_map/set. Must come after every hash()
// overload: ADL alone would miss py::hash for std::string keys.
template <typename T>
struct hasher {
    size_t operator()(const T &x) const { return hash(x); }
};

} // namespace py
