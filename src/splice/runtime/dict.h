#pragma once

// Python dict semantics on top of std::unordered_map.
// Unlike CPython (insertion-ordered since 3.7), iteration order here is
// unspecified, so transpiled programs must sort before comparing.

#include "exceptions.h"
#include "hash.h"
#include "list.h"
#include "str.h"
#include "tuple.h"
#include "types.h"
#include <initializer_list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace py {

template <typename K, typename V> class dict {
  public:
    using key_type = K;
    using mapped_type = V;
    using size_type = _int;

    dict() = default;
    dict(std::initializer_list<std::pair<const K, V>> init) : data_(init) {}

    // dict(pairs): from a list of (key, value) tuples, like Python's
    // dict([(k, v), ...]).
    dict(const list<tuple<K, V>> &pairs) {
        _int n = pairs.__len__();
        for (_int i = 0; i < n; ++i) {
            const tuple<K, V> &pair = pairs[i];
            data_[pair.first()] = pair.second();
        }
    }

    ALWAYS_INLINE size_type __len__() const noexcept {
        return static_cast<size_type>(data_.size());
    }
    ALWAYS_INLINE size_type len() const noexcept { return __len__(); }
    ALWAYS_INLINE bool empty() const noexcept { return data_.empty(); }
    ALWAYS_INLINE explicit operator bool() const noexcept { return !data_.empty(); }

    // Inserts on a missing key, like std::unordered_map::operator[].
    ALWAYS_INLINE V &operator[](const K &key) { return data_[key]; }
    // const can't auto-insert, so this raises KeyError instead, like __getitem__.
    ALWAYS_INLINE const V &operator[](const K &key) const { return __getitem__(key); }

    // d[k] read: raises KeyError on a missing key.
    ALWAYS_INLINE V &__getitem__(const K &key) {
        auto it = data_.find(key);
        if (it == data_.end())
            throw KeyError("key not found");
        return it->second;
    }
    ALWAYS_INLINE const V &__getitem__(const K &key) const {
        auto it = data_.find(key);
        if (it == data_.end())
            throw KeyError("key not found");
        return it->second;
    }

    ALWAYS_INLINE void __setitem__(const K &key, const V &value) { data_[key] = value; }

    // del d[k]
    ALWAYS_INLINE void __delitem__(const K &key) {
        if (data_.erase(key) == 0)
            throw KeyError("key not found");
    }

    ALWAYS_INLINE bool __contains__(const K &key) const { // `k in d`
        return data_.find(key) != data_.end();
    }

    // get(key, default=None): never raises; without a default a missing
    // key yields a value-initialized V.
    ALWAYS_INLINE V get(const K &key) const {
        auto it = data_.find(key);
        return it == data_.end() ? V() : it->second;
    }
    ALWAYS_INLINE V get(const K &key, const V &fallback) const {
        auto it = data_.find(key);
        return it == data_.end() ? fallback : it->second;
    }

    // pop(key[, default]): raises KeyError when absent and no default.
    ALWAYS_INLINE V pop(const K &key) {
        auto it = data_.find(key);
        if (it == data_.end())
            throw KeyError("key not found");
        V value = std::move(it->second);
        data_.erase(it);
        return value;
    }
    ALWAYS_INLINE V pop(const K &key, const V &fallback) {
        auto it = data_.find(key);
        if (it == data_.end())
            return fallback;
        V value = std::move(it->second);
        data_.erase(it);
        return value;
    }

    // CPython pops LIFO; with no insertion order we pop an arbitrary item.
    ALWAYS_INLINE tuple<K, V> popitem() {
        if (data_.empty())
            throw KeyError("popitem(): dictionary is empty");
        auto it = data_.begin();
        tuple<K, V> entry(it->first, it->second);
        data_.erase(it);
        return entry;
    }

    // setdefault(key, default=None): insert-and-return when absent.
    ALWAYS_INLINE V &setdefault(const K &key) { return data_[key]; }
    ALWAYS_INLINE V &setdefault(const K &key, const V &fallback) {
        auto it = data_.find(key);
        if (it == data_.end())
            it = data_.emplace(key, fallback).first;
        return it->second;
    }

    void update(const dict<K, V> &other) {
        for (const auto &entry : other.data_)
            data_[entry.first] = entry.second;
    }

    ALWAYS_INLINE void clear() noexcept { data_.clear(); }

    ALWAYS_INLINE dict<K, V> copy() const { return *this; }

    // Snapshots, not CPython's live views.
    list<K> keys() const {
        list<K> out;
        for (const auto &entry : data_)
            out.append(entry.first);
        return out;
    }
    list<V> values() const {
        list<V> out;
        for (const auto &entry : data_)
            out.append(entry.second);
        return out;
    }
    list<tuple<K, V>> items() const {
        list<tuple<K, V>> out;
        for (const auto &entry : data_)
            out.append(tuple<K, V>(entry.first, entry.second));
        return out;
    }

    // Order-independent, like Python.
    ALWAYS_INLINE bool operator==(const dict<K, V> &o) const { return data_ == o.data_; }
    ALWAYS_INLINE bool operator!=(const dict<K, V> &o) const { return data_ != o.data_; }

    // `for k in d` iterates keys, matching Python - dereferences to the key,
    // not the (key, value) pair a raw std::unordered_map::iterator would.
    class dict_iterator {
      public:
        using map_type = std::unordered_map<K, V, hasher<K>>;

        explicit dict_iterator(typename map_type::const_iterator it) : it_(it) {}
        ALWAYS_INLINE const K &operator*() const { return it_->first; }
        ALWAYS_INLINE dict_iterator &operator++() {
            ++it_;
            return *this;
        }
        ALWAYS_INLINE bool operator!=(const dict_iterator &o) const { return it_ != o.it_; }
        ALWAYS_INLINE bool operator==(const dict_iterator &o) const { return it_ == o.it_; }

      private:
        typename map_type::const_iterator it_;
    };
    ALWAYS_INLINE dict_iterator begin() const { return dict_iterator(data_.begin()); }
    ALWAYS_INLINE dict_iterator end() const { return dict_iterator(data_.end()); }

    ALWAYS_INLINE const std::unordered_map<K, V, hasher<K>> &raw() const noexcept {
        return data_;
    }

    str __str__() const {
        str result = "{";
        bool first = true;
        for (const auto &entry : data_) {
            if (!first)
                result += ", ";
            result += repr(entry.first) + ": " + repr(entry.second);
            first = false;
        }
        return result + "}";
    }

  private:
    std::unordered_map<K, V, hasher<K>> data_;
};

template <typename K, typename V>
ALWAYS_INLINE _int len(const dict<K, V> &d) {
    return d.__len__();
}

template <typename K, typename V> list<K> sorted(const dict<K, V> &d, bool reverse) {
    auto out = d.keys();
    out.sort(reverse);
    return out;
}

template <typename K, typename V, typename KeyFunc>
list<K> sorted(const dict<K, V> &d, KeyFunc key, bool reverse) {
    auto out = d.keys();
    out.sort(key, reverse);
    return out;
}

// str() - {k: v, ...} with elements rendered via repr(), like Python
} // namespace py
