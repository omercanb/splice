#pragma once

#include "exceptions.h"
#include "str.h"
#include "tuple.h"
#include "types.h"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>

namespace py {

class slice {
    std::optional<_int> start;
    std::optional<_int> stop;
    std::optional<_int> step;

  public:
    slice(std::optional<_int> start, std::optional<_int> stop, std::optional<_int> step)
        : start(start), stop(stop), step(step) {
    }

    inline tuple<_int, _int, _int> indices(_int length) const {
        _int istep = step.value_or(1);
        if (istep == 0) {
            throw ValueError("slice step cannot be zero");
        }

        _int istart, istop;

        if (istep > 0) {
            // start
            if (!start.has_value()) {
                istart = 0;
            } else if (*start < 0) {
                istart = std::max(length + *start, static_cast<_int>(0));
            } else {
                istart = std::min(*start, length);
            }
            // stop
            if (!stop.has_value()) {
                istop = length;
            } else if (*stop < 0) {
                istop = std::max(length + *stop, static_cast<_int>(0));
            } else {
                istop = std::min(*stop, length);
            }
        } else {
            // start
            if (!start.has_value()) {
                istart = length - 1;
            } else if (*start < 0) {
                istart = std::max(length + *start, static_cast<_int>(-1));
            } else {
                istart = std::min(*start, length - 1);
            }
            // stop
            if (!stop.has_value()) {
                istop = -1;
            } else if (*stop < 0) {
                istop = std::max(length + *stop, static_cast<_int>(-1));
            } else {
                istop = std::min(*stop, length - 1);
            }
        }

        return tuple<_int, _int, _int>(istart, istop, istep);
    }
};

// Out of line: str.h cannot include this header, because slice needs tuple and
// tuple needs str. str only forward declares slice.
inline str str::__getslice__(const slice &s) const {
    auto bounds = s.indices(__len__());
    _int start = bounds.get<0>(), stop = bounds.get<1>(), step = bounds.get<2>();

    std::string out;
    if (step > 0) {
        for (_int i = start; i < stop; i += step)
            out += data_[static_cast<std::size_t>(i)];
    } else {
        for (_int i = start; i > stop; i += step)
            out += data_[static_cast<std::size_t>(i)];
    }
    return str(std::move(out));
}

} // namespace py
