// Claude generated C++ List that aims to copy python's list semantics

#pragma once

#include "exceptions.h"
#include "iter.h"
#include "range.h"
#include "slice.h"
#include "str.h"
#include "types.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace py {

namespace detail {

// std::vector<bool> is a bad case. It tries to be efficient by bit packing. So we can't use it and we have to define it be a std::vector<char> and we recast the types to bools
class bool_vector {
  public:
    using iterator = std::vector<char>::iterator;
    using const_iterator = std::vector<char>::const_iterator;

    bool_vector() = default;
    bool_vector(std::initializer_list<bool> init) {
        data_.reserve(init.size());
        for (bool b : init)
            data_.push_back(b);
    }

    size_t size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }
    void clear() noexcept { data_.clear(); }

    bool &operator[](size_t i) { return reinterpret_cast<bool &>(data_[i]); }
    const bool &operator[](size_t i) const {
        return reinterpret_cast<const bool &>(data_[i]);
    }
    bool &back() { return reinterpret_cast<bool &>(data_.back()); }
    const bool &back() const { return reinterpret_cast<const bool &>(data_.back()); }

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    void reserve(size_t n) { data_.reserve(n); }
    void push_back(bool b) { data_.push_back(b); }
    iterator insert(iterator pos, bool value) { return data_.insert(pos, value); }
    template <typename It>
    iterator insert(iterator pos, It first, It last) {
        return data_.insert(pos, first, last);
    }
    iterator erase(iterator pos) { return data_.erase(pos); }

  private:
    std::vector<char> data_;
};

inline bool operator==(const bool_vector &a, const bool_vector &b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i])
            return false;
    return true;
}
inline bool operator!=(const bool_vector &a, const bool_vector &b) { return !(a == b); }
inline bool operator<(const bool_vector &a, const bool_vector &b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}
inline bool operator<=(const bool_vector &a, const bool_vector &b) { return !(b < a); }
inline bool operator>(const bool_vector &a, const bool_vector &b) { return b < a; }
inline bool operator>=(const bool_vector &a, const bool_vector &b) { return !(a < b); }

template <typename T>
struct list_storage {
    using type = std::vector<T>;
};
template <>
struct list_storage<bool> {
    using type = bool_vector;
};

} // namespace detail

template <typename T>
class list {
  public:
    using value_type = T;
    using size_type = _int;

    list() = default;
    list(std::initializer_list<T> init) : data_(init) {}

    // Construct from any iterable - requires explicit type: list<int>(map(...))
    // (deduction guide in iter.h handles type inference)
    //
    // enable_if turns this constructor off when IterableType is list<T>
    // itself. Without it, a plain list<T> lvalue would also match this
    // template, and it would beat the real copy constructor: binding a
    // non-const lvalue here needs no extra qualification, while the copy
    // constructor's `const list<T>&` does, so the copy constructor loses.
    // `list<T> b = a;` would then rebuild b element by element through the
    // iterator protocol instead of copying it.
    template <typename IterableType,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<IterableType>, list<T>>>>
    list(IterableType &&iterable) {
        for (auto &&item : iterable)
            data_.push_back(item);
    }

    ALWAYS_INLINE size_type __len__() const noexcept {
        return static_cast<size_type>(data_.size());
    }
    ALWAYS_INLINE bool empty() const noexcept { return data_.empty(); }

    ALWAYS_INLINE T &operator[](size_type i) { return data_[checkIndex(i)]; }
    ALWAYS_INLINE const T &operator[](size_type i) const { return data_[checkIndex(i)]; }

    // What generated code calls for a[i] and a[i] = x. Strict, unlike
    // dict's insert-on-write.
    ALWAYS_INLINE T &__getitem__(size_type i) { return data_[checkIndex(i)]; }
    ALWAYS_INLINE const T &__getitem__(size_type i) const { return data_[checkIndex(i)]; }

    // a[-1] and a[-1] = x. A reference, like operator[], so both read and
    // write go through the same call.
    ALWAYS_INLINE T &back() {
        if (data_.empty())
            throw IndexError("list index out of range");
        return data_.back();
    }
    ALWAYS_INLINE const T &back() const {
        if (data_.empty())
            throw IndexError("list index out of range");
        return data_.back();
    }

    // a[i:j:k] -- a new list, like Python. Out of range bounds clamp rather
    // than raising, which is why this does not go through checkIndex.
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
    ALWAYS_INLINE void __setitem__(size_type i, const T &value) {
        data_[checkIndex(i)] = value;
    }

    ALWAYS_INLINE void __delitem__(size_type i) { // del a[i]  (strict)
        data_.erase(data_.begin() + checkIndex(i));
    }

    ALWAYS_INLINE void append(const T &x) { data_.push_back(x); }
    ALWAYS_INLINE void append(T &&x) { data_.push_back(std::move(x)); }

    // Clamps instead of raising: insert(len, x) == append.
    ALWAYS_INLINE void insert(_int i, const T &x) {
        _int n = __len__();
        if (i < 0) {
            i += n;
            if (i < 0)
                i = 0;
        }
        if (i > n)
            i = n;
        data_.insert(data_.begin() + i, x);
    }

    void remove(const T &value) {
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            if (*it == value) {
                data_.erase(it);
                return;
            }
        }
        throw ValueError("list.remove(x): x not in list");
    }

    ALWAYS_INLINE T pop(_int i = -1) {
        if (data_.empty())
            throw IndexError("pop from empty list");
        _int n = __len__();
        if (i < 0)
            i += n;
        if (i < 0 || i >= n)
            throw IndexError("pop index out of range");
        T value = std::move(data_[static_cast<std::size_t>(i)]);
        data_.erase(data_.begin() + i);
        return value;
    }

    void extend(const list<T> &other) {
        auto len = other.__len__();
        for (size_type i = 0; i < len; ++i) {
            this->append(other[i]);
        }
    }

    ALWAYS_INLINE void clear() noexcept { data_.clear(); }

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
        std::ostringstream m;
        m << "list.index(x): x not in list";
        throw ValueError(m.str());
    }

    _int count(const T &value) const {
        _int c = 0;
        for (const auto &e : data_)
            if (e == value)
                ++c;
        return c;
    }

    // sort: stable (like CPython). reverse keeps equal elements' original
    // order.
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

    ALWAYS_INLINE list<T> copy() const { return *this; }

    bool __contains__(const T &value) const { // `value in a`
        for (const auto &e : data_)
            if (e == value)
                return true;
        return false;
    }
    ALWAYS_INLINE auto begin() { return data_.begin(); }
    ALWAYS_INLINE auto end() { return data_.end(); }
    ALWAYS_INLINE auto begin() const { return data_.begin(); }
    ALWAYS_INLINE auto end() const { return data_.end(); }

    // + returns a new list; += extends in place and returns *this (Python
    // semantics).
    ALWAYS_INLINE list<T> operator+(const list<T> &other) const {
        list<T> out;
        out.data_ = data_;
        out.data_.insert(out.data_.end(), other.data_.begin(),
                         other.data_.end());
        return out;
    }
    ALWAYS_INLINE list<T> &operator+=(const list<T> &other) {
        data_.insert(data_.end(), other.data_.begin(), other.data_.end());
        return *this;
    }

    // * repetition. Non-positive count yields an empty list (Python behavior).
    list<T> operator*(size_type n) const {
        list<T> out;
        if (n > 0) {
            out.data_.reserve(static_cast<std::size_t>(n * __len__()));
            for (_int k = 0; k < n; ++k)
                out.data_.insert(out.data_.end(), data_.begin(), data_.end());
        }
        return out;
    }
    list<T> &operator*=(size_type n) {
        *this = (*this) * n;
        return *this;
    }

    // Lexicographic comparison (std::vector already does this element-wise).
    ALWAYS_INLINE bool operator==(const list<T> &o) const { return data_ == o.data_; }
    ALWAYS_INLINE bool operator!=(const list<T> &o) const { return data_ != o.data_; }
    ALWAYS_INLINE bool operator<(const list<T> &o) const { return data_ < o.data_; }
    ALWAYS_INLINE bool operator<=(const list<T> &o) const { return data_ <= o.data_; }
    ALWAYS_INLINE bool operator>(const list<T> &o) const { return data_ > o.data_; }
    ALWAYS_INLINE bool operator>=(const list<T> &o) const { return data_ >= o.data_; }

    ALWAYS_INLINE const typename detail::list_storage<T>::type &raw() const noexcept {
        return data_;
    } // escape hatch

    str __str__() const {
        str result = "[";
        for (size_type i = 0; i < __len__(); ++i) {
            if (i > 0)
                result += ", ";
            result += repr((*this)[i]);
        }
        return result + "]";
    }

  private:
    typename detail::list_storage<T>::type data_;

    // strict bounds check shared by [], delItem - no negative-index
    // wraparound, since only -1 (handled separately via back()) is supported
    ALWAYS_INLINE std::size_t checkIndex(_int i) const {
        _int n = __len__();
        if (i < 0 || i >= n)
            throw IndexError("list index out of range");
        return static_cast<std::size_t>(i);
    }
};

// n * a  (mirror of a * n)
template <typename T>
list<T> operator*(typename list<T>::size_type n, const list<T> &a) {
    return a * n;
}

template <typename T>
ALWAYS_INLINE _int len(const list<T> &l) {
    return l.__len__();
}

template <typename T>
list<T> sorted(const list<T> &l, bool reverse) {
    auto out = l.copy();
    out.sort(reverse);
    return out;
}

template <typename T, typename KeyFunc>
list<T> sorted(const list<T> &l, KeyFunc key, bool reverse) {
    auto out = l.copy();
    out.sort(key, reverse);
    return out;
}

// Deduction guide
template <typename IterableType>
list(IterableType &&)
    -> list<std::decay_t<decltype(*std::declval<IterableType &>().begin())>>;

} // namespace py
