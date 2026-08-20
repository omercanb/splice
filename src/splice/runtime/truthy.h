#pragma once

#include "types.h"
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace py {

// Scalars and strings get concrete overloads; everything else falls back
// to operator bool(), then __bool__(), then __len__() != 0, then True -
// mirroring CPython's resolution order.

ALWAYS_INLINE bool to_bool(_int x) { return x != 0; }
ALWAYS_INLINE bool to_bool(_float x) { return x != 0.0; }
ALWAYS_INLINE bool to_bool(bool x) { return x; }
ALWAYS_INLINE bool to_bool(const std::string &s) { return !s.empty(); }

// None is falsy; otherwise defer to the held value's own truthiness
template <typename T>
ALWAYS_INLINE bool to_bool(const std::optional<T> &x) {
    return x.has_value() && to_bool(*x);
}

// The traits detect through a mutable reference: a transpiled class writes its
// methods without const, the way Python does, and requiring it here would make
// every user class look like it had no __bool__ or __len__ at all.
namespace detail {

template <typename T, typename = void>
struct has_bool_conversion : std::false_type {};
template <typename T>
struct has_bool_conversion<
    T, std::void_t<decltype(static_cast<bool>(std::declval<T &>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_bool_method : std::false_type {};
template <typename T>
struct has_bool_method<
    T, std::void_t<decltype(std::declval<T &>().__bool__())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_len_method : std::false_type {};
template <typename T>
struct has_len_method<
    T, std::void_t<decltype(std::declval<T &>().__len__())>>
    : std::true_type {};

} // namespace detail

template <typename T>
ALWAYS_INLINE bool to_bool(const T &x) {
    T &value = const_cast<T &>(x);
    if constexpr (detail::has_bool_conversion<T>::value) {
        return static_cast<bool>(value);
    } else if constexpr (detail::has_bool_method<T>::value) {
        return value.__bool__();
    } else if constexpr (detail::has_len_method<T>::value) {
        return value.__len__() != 0;
    } else {
        return true;
    }
}

} // namespace py

// `and`/`or` as a value (conditions use && / || directly instead). Macros,
// not functions, so evaluation stays short-circuited.
#define _and(a, b) ([&] { auto lhs = (a); return py::to_bool(lhs) ? (b) : lhs; }())
#define _or(a, b) ([&] { auto lhs = (a); return py::to_bool(lhs) ? lhs : (b); }())
