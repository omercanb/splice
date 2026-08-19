#pragma once

// str methods that return a list or tuple, defined here because list.h and
// tuple.h both include str.h and so cannot be included from it.

#include "list.h"
#include "str.h"
#include "tuple.h"

namespace py {

// No separator splits on runs of whitespace and drops empty pieces;
// an explicit separator keeps them. Python draws the same distinction.
inline list<str> str::split() const {
    list<str> out;
    const std::string &s = raw();
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && std::isspace((unsigned char)s[i]))
            ++i;
        size_t start = i;
        while (i < s.size() && !std::isspace((unsigned char)s[i]))
            ++i;
        if (i > start)
            out.append(str(s.substr(start, i - start)));
    }
    return out;
}

inline list<str> str::split(const str &sep) const {
    list<str> out;
    const std::string &s = raw();
    const std::string &d = sep.raw();
    if (d.empty())
        throw ValueError("empty separator");
    size_t prev = 0, at;
    while ((at = s.find(d, prev)) != std::string::npos) {
        out.append(str(s.substr(prev, at - prev)));
        prev = at + d.size();
    }
    out.append(str(s.substr(prev)));
    return out;
}

inline list<str> str::rsplit(const str &sep) const {
    auto parts = split(sep);
    return parts;
}

// A trailing newline does not produce a final empty line.
inline list<str> str::splitlines() const {
    list<str> out;
    const std::string &s = raw();
    size_t start = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\n') {
            out.append(str(s.substr(start, i - start)));
            start = i + 1;
        }
    }
    if (start < s.size())
        out.append(str(s.substr(start)));
    return out;
}

inline str str::join(const list<str> &parts) const {
    std::string out;
    _int n = parts.__len__();
    for (_int i = 0; i < n; ++i) {
        if (i)
            out += raw();
        out += parts.__getitem__(i).raw();
    }
    return str(std::move(out));
}

// Splits on the first (partition) or last (rpartition) occurrence of sep.
// Absent, Python returns (s, "", "") / ("", "", s) rather than raising.
inline tuple<str, str, str> str::partition(const str &sep) const {
    const std::string &s = raw();
    size_t at = s.find(sep.raw());
    if (at == std::string::npos)
        return tuple<str, str, str>(*this, str(""), str(""));
    return tuple<str, str, str>(str(s.substr(0, at)), sep,
                                str(s.substr(at + sep.raw().size())));
}

inline tuple<str, str, str> str::rpartition(const str &sep) const {
    const std::string &s = raw();
    size_t at = s.rfind(sep.raw());
    if (at == std::string::npos)
        return tuple<str, str, str>(str(""), str(""), *this);
    return tuple<str, str, str>(str(s.substr(0, at)), sep,
                                str(s.substr(at + sep.raw().size())));
}

} // namespace py
