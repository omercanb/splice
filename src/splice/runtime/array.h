#pragma once

#include "str.h"
#include "types.h"
#include <array>
#include <cstddef>

namespace py {

// Fixed-size, backed by std::array
// No negative indexing

template <typename T, std::size_t N>
class Array {
  public:
    using value_type = T;
    using size_type = _int;

    Array() = default;
    explicit Array(const T &fill) { data_.fill(fill); }

    size_type __len__() const noexcept { return static_cast<size_type>(N); }

    T &operator[](size_type i) { return data_[static_cast<std::size_t>(i)]; }
    const T &operator[](size_type i) const {
        return data_[static_cast<std::size_t>(i)];
    }

    T &__getitem__(size_type i) { return data_[static_cast<std::size_t>(i)]; }
    const T &__getitem__(size_type i) const {
        return data_[static_cast<std::size_t>(i)];
    }
    void __setitem__(size_type i, const T &value) {
        data_[static_cast<std::size_t>(i)] = value;
    }

    T &back() { return data_.back(); }
    const T &back() const { return data_.back(); }

    void fill(const T &value) { data_.fill(value); }

    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    bool operator==(const Array<T, N> &o) const { return data_ == o.data_; }
    bool operator!=(const Array<T, N> &o) const { return data_ != o.data_; }

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
inline _int len(const Array<T, N> &a) {
    return a.__len__();
}

} // namespace py
