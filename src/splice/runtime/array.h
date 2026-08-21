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
