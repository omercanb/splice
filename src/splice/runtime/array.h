#pragma once

#include "exceptions.h"
#include "list.h"
#include "slice.h"
#include "str.h"
#include "types.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>

namespace py {

// Fixed-size, backed by std::array
// No negative indexing

template <typename T, std::size_t N>
class Array {
  public:
    using value_type = T;
    using size_type = _int;

    Array() = default;
    ALWAYS_INLINE explicit Array(const T &fill) { data_.fill(fill); }

    ALWAYS_INLINE size_type __len__() const noexcept { return static_cast<size_type>(N); }

    ALWAYS_INLINE T &operator[](size_type i) { return data_[static_cast<std::size_t>(i)]; }
    ALWAYS_INLINE const T &operator[](size_type i) const {
        return data_[static_cast<std::size_t>(i)];
    }

    ALWAYS_INLINE T &__getitem__(size_type i) { return data_[static_cast<std::size_t>(i)]; }
    ALWAYS_INLINE const T &__getitem__(size_type i) const {
        return data_[static_cast<std::size_t>(i)];
    }
    ALWAYS_INLINE void __setitem__(size_type i, const T &value) {
        data_[static_cast<std::size_t>(i)] = value;
    }

    ALWAYS_INLINE T &back() { return data_.back(); }
    ALWAYS_INLINE const T &back() const { return data_.back(); }

    ALWAYS_INLINE T *data() { return data_.data(); }
    ALWAYS_INLINE const T *data() const { return data_.data(); }

    ALWAYS_INLINE void fill(const T &value) { data_.fill(value); }

    bool __contains__(const T &value) const { // `value in a`
        for (const auto &e : data_)
            if (e == value)
                return true;
        return false;
    }

    _int index(const T &value, size_type start = 0,
               std::optional<size_type> stop = std::nullopt) const {
        _int n = __len__();
        _int s = start;
        _int e = stop.value_or(n);
        if (s < 0) {
            s += n;
            if (s < 0)
                s = 0;
        }
        if (e < 0) {
            e += n;
        } // note: not clamped to 0, matching CPython
        else if (e > n)
            e = n;
        for (_int k = s; k < e; ++k) {
            if (data_[static_cast<std::size_t>(k)] == value)
                return k;
        }
        throw ValueError("array.index(x): x not in array");
    }

    _int count(const T &value) const {
        _int c = 0;
        for (const auto &e : data_)
            if (e == value)
                ++c;
        return c;
    }

    // sort: stable (like CPython). reverse keeps equal elements' original order.
    void sort(bool reverse = false) {
        if (!reverse)
            std::stable_sort(data_.begin(), data_.end(),
                             [](const T &a, const T &b) { return a < b; });
        else
            std::stable_sort(data_.begin(), data_.end(),
                             [](const T &a, const T &b) { return b < a; });
    }

    // sort by key(element)
    template <typename KeyFunc>
    void sort(KeyFunc key, bool reverse) {
        if (!reverse)
            std::stable_sort(data_.begin(), data_.end(),
                             [&](const T &a, const T &b) { return key(a) < key(b); });
        else
            std::stable_sort(data_.begin(), data_.end(),
                             [&](const T &a, const T &b) { return key(b) < key(a); });
    }

    ALWAYS_INLINE void reverse() noexcept { std::reverse(data_.begin(), data_.end()); }

    // a[i:j:k] -- a new list, since a slice's length isn't known at compile
    // time so it can't stay an Array. Out of range bounds clamp rather than
    // raising, matching list<T>'s __getslice__.
    list<T> __getslice__(const slice &s) const {
        tuple<_int, _int, _int> bounds = s.indices(__len__());
        _int start = bounds.get<0>(), stop = bounds.get<1>(),
             step = bounds.get<2>();

        list<T> out;
        if (step > 0) {
            for (_int i = start; i < stop; i += step)
                out.append(data_[static_cast<std::size_t>(i)]);
        } else {
            for (_int i = start; i > stop; i += step)
                out.append(data_[static_cast<std::size_t>(i)]);
        }
        return out;
    }
    ALWAYS_INLINE list<T> operator[](const slice &s) const { return __getslice__(s); }

    ALWAYS_INLINE auto begin() { return data_.begin(); }
    ALWAYS_INLINE auto end() { return data_.end(); }
    ALWAYS_INLINE auto begin() const { return data_.begin(); }
    ALWAYS_INLINE auto end() const { return data_.end(); }

    ALWAYS_INLINE bool operator==(const Array<T, N> &o) const { return data_ == o.data_; }
    ALWAYS_INLINE bool operator!=(const Array<T, N> &o) const { return data_ != o.data_; }

    str __str__() const {
        str result = "[";
        for (size_type i = 0; i < __len__(); ++i) {
            if (i > 0)
                result += ", ";
            result += repr(data_[static_cast<std::size_t>(i)]);
        }
        return result + "]";
    }

  private:
    std::array<T, N> data_;
};

template <typename T, std::size_t N>
ALWAYS_INLINE _int len(const Array<T, N> &a) {
    return a.__len__();
}

} // namespace py
