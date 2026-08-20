#pragma once

// Python set semantics on top of std::unordered_set.
// Python guarantees no ordering here either, so programs must sort before
// comparing. frozenset is not implemented.

#include "exceptions.h"
#include "hash.h"
#include "list.h"
#include "str.h"
#include "types.h"
#include <initializer_list>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace py {

template <typename T> class set {
  public:
    using value_type = T;
    using size_type = _int;

    set() = default;
    set(std::initializer_list<T> init) : data_(init) {}

    // enable_if turns this constructor off when IterableType is set<T>
    // itself. Without it, a plain set<T> lvalue would also match this
    // template, and it would beat the real copy constructor: binding a
    // non-const lvalue here needs no extra qualification, while the copy
    // constructor's `const set<T>&` does, so the copy constructor loses.
    // `set<T> b = a;` would then rebuild b element by element through the
    // iterator protocol instead of copying it.
    template <typename IterableType,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<IterableType>, set<T>>>>
    set(IterableType &&iterable) {
        for (auto &&item : iterable)
            data_.insert(item);
    }

    ALWAYS_INLINE size_type __len__() const noexcept {
        return static_cast<size_type>(data_.size());
    }

    ALWAYS_INLINE bool __contains__(const T &value) const {
        return data_.find(value) != data_.end();
    }

    ALWAYS_INLINE void add(const T &value) { data_.insert(value); }

    ALWAYS_INLINE void remove(const T &value) {
        if (data_.erase(value) == 0)
            throw KeyError("element not found");
    }
    ALWAYS_INLINE void discard(const T &value) { data_.erase(value); }

    ALWAYS_INLINE T pop() {
        if (data_.empty())
            throw KeyError("pop from an empty set");
        auto it = data_.begin();
        T value = *it;
        data_.erase(it);
        return value;
    }

    ALWAYS_INLINE void clear() noexcept { data_.clear(); }

    void update(const set<T> &other) {
        for (const auto &v : other.data_)
            data_.insert(v);
    }
    void intersection_update(const set<T> &other) {
        std::unordered_set<T, hasher<T>> kept;
        for (const auto &v : data_)
            if (other.__contains__(v))
                kept.insert(v);
        data_ = std::move(kept);
    }
    void difference_update(const set<T> &other) {
        for (const auto &v : other.data_)
            data_.erase(v);
    }
    void symmetric_difference_update(const set<T> &other) {
        for (const auto &v : other.data_) {
            if (data_.find(v) != data_.end())
                data_.erase(v);
            else
                data_.insert(v);
        }
    }

    // `union` is a C++ keyword; codegen rewrites s.union(...) to union_.
    set<T> union_(const set<T> &other) const {
        set<T> out = *this;
        out.update(other);
        return out;
    }
    set<T> intersection(const set<T> &other) const {
        set<T> out;
        for (const auto &v : data_)
            if (other.__contains__(v))
                out.add(v);
        return out;
    }
    set<T> difference(const set<T> &other) const {
        set<T> out;
        for (const auto &v : data_)
            if (!other.__contains__(v))
                out.add(v);
        return out;
    }
    set<T> symmetric_difference(const set<T> &other) const {
        set<T> out = *this;
        out.symmetric_difference_update(other);
        return out;
    }

    ALWAYS_INLINE set<T> copy() const { return *this; }

    bool issubset(const set<T> &other) const {
        for (const auto &v : data_)
            if (!other.__contains__(v))
                return false;
        return true;
    }
    bool issuperset(const set<T> &other) const {
        for (const auto &v : other.raw())
            if (!__contains__(v))
                return false;
        return true;
    }
    bool isdisjoint(const set<T> &other) const {
        for (const auto &v : data_)
            if (other.__contains__(v))
                return false;
        return true;
    }

    ALWAYS_INLINE bool operator==(const set<T> &o) const { return data_ == o.data_; }
    ALWAYS_INLINE bool operator!=(const set<T> &o) const { return data_ != o.data_; }
    bool operator<=(const set<T> &o) const {
        for (const auto &v : data_)
            if (o.data_.find(v) == o.data_.end())
                return false;
        return true;
    }
    bool operator<(const set<T> &o) const {
        return __len__() < o.__len__() && *this <= o;
    }
    bool operator>=(const set<T> &o) const { return o <= *this; }
    bool operator>(const set<T> &o) const { return o < *this; }

    using const_iterator = typename std::unordered_set<T, hasher<T>>::const_iterator;
    ALWAYS_INLINE const_iterator begin() const { return data_.begin(); }
    ALWAYS_INLINE const_iterator end() const { return data_.end(); }

    ALWAYS_INLINE const std::unordered_set<T, hasher<T>> &raw() const noexcept {
        return data_;
    }

    str __str__() const {
        if (__len__() == 0)
            return str("set()");
        str result = "{";
        bool first = true;
        for (const auto &v : data_) {
            if (!first)
                result += ", ";
            result += repr(v);
            first = false;
        }
        return result + "}";
    }

  private:
    std::unordered_set<T, hasher<T>> data_;
};

template <typename T> ALWAYS_INLINE _int len(const set<T> &s) { return s.__len__(); }

template <typename T>
set<T> operator|(const set<T> &a, const set<T> &b) {
    return a.union_(b);
}
template <typename T>
set<T> operator&(const set<T> &a, const set<T> &b) {
    return a.intersection(b);
}
template <typename T>
set<T> operator-(const set<T> &a, const set<T> &b) {
    return a.difference(b);
}
template <typename T>
set<T> operator^(const set<T> &a, const set<T> &b) {
    return a.symmetric_difference(b);
}

// sorted() is how a program gets a stable view of an unordered set.
template <typename T> list<T> sorted(const set<T> &s) {
    list<T> out;
    for (const auto &v : s.raw())
        out.append(v);
    out.sort();
    return out;
}
template <typename T>
list<T> _sorted_kwargs(bool reverse, const set<T> &s) {
    auto out = sorted(s);
    out.sort(reverse);
    return out;
}

// str() - {1, 2, 3}; empty prints as set(), since {} is an empty dict.
} // namespace py
