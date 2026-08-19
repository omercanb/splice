#pragma once

// copy() used for mutable value semantics

namespace py {

template <typename T>
inline T copy(const T &value) {
    return value;
}

} // namespace py
