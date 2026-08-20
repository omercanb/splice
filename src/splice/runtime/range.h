#pragma once

#include "str.h"
#include "types.h"
#include <climits>
#include <iostream>
#include <string>

namespace py {

class range_iterator;
class range;
std::ostream &operator<<(std::ostream &os, const range &r);

class range_iterator {
  public:
    range_iterator(_int cur, _int step) : cur_(cur), step_(step) {}
    _int operator*() const { return cur_; }
    range_iterator &operator++() {
        cur_ += step_;
        return *this;
    }
    bool operator!=(const range_iterator &o) const {
        return step_ > 0 ? cur_ < o.cur_ : cur_ > o.cur_;
    }
    bool operator==(const range_iterator &o) const { return !(*this != o); }

  private:
    _int cur_;
    _int step_;
};

class range {
  public:
    _int start;
    _int stop;
    _int step;
    range() { start = stop = step = 0; }
    range(_int start, _int stop = INT_MAX, _int step = 1) {
        if (stop == INT_MAX) {
            stop = start;
            start = 0;
        }
        this->start = start;
        this->stop = stop;
        this->step = step;
    }
    range_iterator begin() const { return range_iterator(start, step); }
    range_iterator end() const { return range_iterator(stop, step); }
    str __str__() const;
};

std::ostream &operator<<(std::ostream &os, const range &r) {
    os << "range(" << r.start << ", " << r.stop;
    if (r.step != 1) {
        os << ", " << r.step;
    }
    os << ")";
    return os;
};

inline str range::__str__() const {
    std::string result =
        "range(" + std::to_string(start) + ", " + std::to_string(stop);
    if (step != 1) {
        result += ", " + std::to_string(step);
    }
    result += ")";
    return str(result);
}

} // namespace py
