#pragma once

// Python bytes on top of std::string. Unlike str, indexing and iteration
// yield ints (Python 3's bytes/int split), and encode()/decode() are not
// implemented: this runtime never converts between str and bytes.

#include "exceptions.h"
#include "list.h"
#include "slice.h"
#include "truthy.h"
#include "tuple.h"
#include "types.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>

namespace py {

namespace detail {
inline std::string bytes_hex_escape(unsigned char c) {
    static const char *hexd = "0123456789abcdef";
    std::string out = "\\x";
    out += hexd[c >> 4];
    out += hexd[c & 0xf];
    return out;
}
} // namespace detail

class bytes {
  public:
    using size_type = _int;

    bytes() = default;
    bytes(const char *s) : data_(s) {}
    bytes(const std::string &s) : data_(s) {}
    bytes(std::string &&s) : data_(std::move(s)) {}
    // bytes(n): n zero bytes, like Python.
    explicit bytes(size_type n) : data_(static_cast<size_t>(n < 0 ? 0 : n), '\0') {}
    // bytes(values): from an iterable of ints, like Python.
    explicit bytes(const list<_int> &values) {
        _int n = values.__len__();
        data_.resize(static_cast<size_t>(n));
        for (_int i = 0; i < n; ++i)
            data_[static_cast<size_t>(i)] = static_cast<char>(values[i]);
    }

    ALWAYS_INLINE const std::string &raw() const noexcept { return data_; }

    // Both render as b'...': printing bytes never shows raw text. Matches
    // CPython's bytes repr exactly, including its quote-choosing rule.
    str __repr__() const {
        bool has_single = data_.find('\'') != std::string::npos;
        bool has_double = data_.find('"') != std::string::npos;
        char quote = (has_single && !has_double) ? '"' : '\'';

        std::string out = "b";
        out += quote;
        for (char ch : data_) {
            unsigned char c = static_cast<unsigned char>(ch);
            if (c == '\\')
                out += "\\\\";
            else if (c == static_cast<unsigned char>(quote))
                out += std::string("\\") + quote;
            else if (c == '\t')
                out += "\\t";
            else if (c == '\n')
                out += "\\n";
            else if (c == '\r')
                out += "\\r";
            else if (c >= 0x20 && c < 0x7f)
                out += static_cast<char>(c);
            else
                out += detail::bytes_hex_escape(c);
        }
        out += quote;
        return str(std::move(out));
    }
    str __str__() const { return __repr__(); }

    ALWAYS_INLINE size_type __len__() const noexcept {
        return static_cast<size_type>(data_.size());
    }
    ALWAYS_INLINE size_type len() const noexcept { return __len__(); }
    ALWAYS_INLINE bool empty() const noexcept { return data_.empty(); }
    ALWAYS_INLINE explicit operator bool() const noexcept { return !data_.empty(); }
    ALWAYS_INLINE size_t __hash__() const { return std::hash<std::string>{}(data_); }

    // Indexing yields the byte's value, unlike str: bytes has no character type.
    ALWAYS_INLINE _int __getitem__(size_type i) const {
        return static_cast<_int>(static_cast<unsigned char>(data_[normIndex(i)]));
    }
    ALWAYS_INLINE _int back() const {
        if (data_.empty())
            throw IndexError("index out of range");
        return static_cast<_int>(static_cast<unsigned char>(data_.back()));
    }
    // b[i:j:k] -- a new bytes.
    bytes __getitem__(const slice &s) const {
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
        return bytes(std::move(out));
    }
    ALWAYS_INLINE bool __contains__(const bytes &sub) const {
        return data_.find(sub.data_) != std::string::npos;
    }
    ALWAYS_INLINE bool __contains__(_int byte) const {
        return data_.find(static_cast<char>(byte)) != std::string::npos;
    }

    bytes upper() const { return mapped(::toupper); }
    bytes lower() const { return mapped(::tolower); }
    bytes swapcase() const {
        std::string out = data_;
        for (char &c : out)
            c = isLower(c) ? up(c) : (isUpper(c) ? low(c) : c);
        return bytes(std::move(out));
    }
    bytes capitalize() const {
        std::string out = lower().data_;
        if (!out.empty())
            out[0] = up(out[0]);
        return bytes(std::move(out));
    }
    bytes title() const {
        std::string out = data_;
        bool start = true;
        for (char &c : out) {
            c = start ? up(c) : low(c);
            start = !isAlpha(c);
        }
        return bytes(std::move(out));
    }

    ALWAYS_INLINE _int find(const bytes &sub) const { return toIndex(data_.find(sub.data_)); }
    ALWAYS_INLINE _int find(const bytes &sub, size_type start) const {
        return toIndex(data_.find(sub.data_, clampStart(start)));
    }
    ALWAYS_INLINE _int rfind(const bytes &sub) const { return toIndex(data_.rfind(sub.data_)); }
    ALWAYS_INLINE _int index(const bytes &sub) const { return found(find(sub)); }
    ALWAYS_INLINE _int rindex(const bytes &sub) const { return found(rfind(sub)); }

    ALWAYS_INLINE bool startswith(const bytes &prefix) const {
        return data_.rfind(prefix.data_, 0) == 0;
    }
    ALWAYS_INLINE bool endswith(const bytes &suffix) const {
        return data_.size() >= suffix.data_.size() &&
               data_.compare(data_.size() - suffix.data_.size(),
                             suffix.data_.size(), suffix.data_) == 0;
    }

    _int count(const bytes &sub) const {
        if (sub.data_.empty())
            return __len__() + 1;
        _int n = 0;
        for (size_t at = data_.find(sub.data_); at != std::string::npos;
             at = data_.find(sub.data_, at + sub.data_.size()))
            ++n;
        return n;
    }

    bytes replace(const bytes &old_b, const bytes &new_b, _int count = -1) const {
        if (old_b.data_.empty())
            return *this;
        std::string out;
        size_t at = 0, prev = 0;
        while (count != 0 && (at = data_.find(old_b.data_, prev)) !=
                                 std::string::npos) {
            out.append(data_, prev, at - prev).append(new_b.data_);
            prev = at + old_b.data_.size();
            if (count > 0)
                --count;
        }
        out.append(data_, prev, std::string::npos);
        return bytes(std::move(out));
    }

    ALWAYS_INLINE bytes removeprefix(const bytes &prefix) const {
        return startswith(prefix) ? bytes(data_.substr(prefix.data_.size()))
                                  : *this;
    }
    ALWAYS_INLINE bytes removesuffix(const bytes &suffix) const {
        return !suffix.data_.empty() && endswith(suffix)
                   ? bytes(data_.substr(0, data_.size() - suffix.data_.size()))
                   : *this;
    }

    ALWAYS_INLINE bytes strip() const { return lstrip().rstrip(); }
    ALWAYS_INLINE bytes lstrip() const {
        size_t b = data_.find_first_not_of(" \t\n\r\f\v");
        return bytes(b == std::string::npos ? "" : data_.substr(b));
    }
    ALWAYS_INLINE bytes rstrip() const {
        size_t e = data_.find_last_not_of(" \t\n\r\f\v");
        return bytes(e == std::string::npos ? "" : data_.substr(0, e + 1));
    }
    ALWAYS_INLINE bytes strip(const bytes &chars) const { return lstrip(chars).rstrip(chars); }
    ALWAYS_INLINE bytes lstrip(const bytes &chars) const {
        size_t b = data_.find_first_not_of(chars.data_);
        return bytes(b == std::string::npos ? "" : data_.substr(b));
    }
    ALWAYS_INLINE bytes rstrip(const bytes &chars) const {
        size_t e = data_.find_last_not_of(chars.data_);
        return bytes(e == std::string::npos ? "" : data_.substr(0, e + 1));
    }

    ALWAYS_INLINE bytes ljust(size_type width, const bytes &fill = bytes(" ")) const {
        return pad(width, fill, 0);
    }
    ALWAYS_INLINE bytes rjust(size_type width, const bytes &fill = bytes(" ")) const {
        return pad(width, fill, 1);
    }
    ALWAYS_INLINE bytes center(size_type width, const bytes &fill = bytes(" ")) const {
        return pad(width, fill, 2);
    }
    ALWAYS_INLINE bytes zfill(size_type width) const {
        _int extra = width - __len__();
        if (extra <= 0)
            return *this;
        bool signed_ = !data_.empty() && (data_[0] == '-' || data_[0] == '+');
        std::string out = signed_ ? std::string(1, data_[0]) : std::string();
        out += std::string(extra, '0');
        out += signed_ ? data_.substr(1) : data_;
        return bytes(std::move(out));
    }

    ALWAYS_INLINE bool isalpha() const { return allOf(isAlpha); }
    ALWAYS_INLINE bool isdigit() const { return allOf(isDigit); }
    ALWAYS_INLINE bool isalnum() const {
        return allOf([](char c) { return isAlpha(c) || isDigit(c); });
    }
    ALWAYS_INLINE bool isspace() const {
        return allOf([](char c) { return std::isspace((unsigned char)c) != 0; });
    }
    ALWAYS_INLINE bool isascii() const {
        return std::all_of(data_.begin(), data_.end(),
                           [](char c) { return (unsigned char)c < 128; });
    }
    bool isupper() const { return casedAllAre(true); }
    bool islower() const { return casedAllAre(false); }

    list<bytes> split() const { // on whitespace
        list<bytes> out;
        const std::string &s = data_;
        size_t i = 0;
        while (i < s.size()) {
            while (i < s.size() && std::isspace((unsigned char)s[i]))
                ++i;
            size_t start = i;
            while (i < s.size() && !std::isspace((unsigned char)s[i]))
                ++i;
            if (i > start)
                out.append(bytes(s.substr(start, i - start)));
        }
        return out;
    }
    list<bytes> split(const bytes &sep) const {
        list<bytes> out;
        const std::string &s = data_;
        const std::string &d = sep.data_;
        if (d.empty())
            throw ValueError("empty separator");
        size_t prev = 0, at;
        while ((at = s.find(d, prev)) != std::string::npos) {
            out.append(bytes(s.substr(prev, at - prev)));
            prev = at + d.size();
        }
        out.append(bytes(s.substr(prev)));
        return out;
    }
    list<bytes> rsplit(const bytes &sep) const { return split(sep); }
    list<bytes> splitlines() const {
        list<bytes> out;
        const std::string &s = data_;
        size_t start = 0;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\n') {
                out.append(bytes(s.substr(start, i - start)));
                start = i + 1;
            }
        }
        if (start < s.size())
            out.append(bytes(s.substr(start)));
        return out;
    }
    bytes join(const list<bytes> &parts) const {
        std::string out;
        _int n = parts.__len__();
        for (_int i = 0; i < n; ++i) {
            if (i)
                out += data_;
            out += parts.__getitem__(i).raw();
        }
        return bytes(std::move(out));
    }
    ALWAYS_INLINE tuple<bytes, bytes, bytes> partition(const bytes &sep) const {
        size_t at = data_.find(sep.data_);
        if (at == std::string::npos)
            return tuple<bytes, bytes, bytes>(*this, bytes(""), bytes(""));
        return tuple<bytes, bytes, bytes>(bytes(data_.substr(0, at)), sep,
                                          bytes(data_.substr(at + sep.data_.size())));
    }
    ALWAYS_INLINE tuple<bytes, bytes, bytes> rpartition(const bytes &sep) const {
        size_t at = data_.rfind(sep.data_);
        if (at == std::string::npos)
            return tuple<bytes, bytes, bytes>(bytes(""), bytes(""), *this);
        return tuple<bytes, bytes, bytes>(bytes(data_.substr(0, at)), sep,
                                          bytes(data_.substr(at + sep.data_.size())));
    }

    ALWAYS_INLINE bytes operator+(const bytes &o) const { return bytes(data_ + o.data_); }
    ALWAYS_INLINE bytes &operator+=(const bytes &o) {
        data_ += o.data_;
        return *this;
    }
    bytes operator*(_int n) const {
        std::string out;
        for (_int k = 0; k < n; ++k)
            out += data_;
        return bytes(std::move(out));
    }
    ALWAYS_INLINE bool operator==(const bytes &o) const { return data_ == o.data_; }
    ALWAYS_INLINE bool operator!=(const bytes &o) const { return data_ != o.data_; }
    ALWAYS_INLINE bool operator<(const bytes &o) const { return data_ < o.data_; }
    ALWAYS_INLINE bool operator<=(const bytes &o) const { return data_ <= o.data_; }
    ALWAYS_INLINE bool operator>(const bytes &o) const { return data_ > o.data_; }
    ALWAYS_INLINE bool operator>=(const bytes &o) const { return data_ >= o.data_; }

    // Iterating bytes yields byte values as ints, unlike str.
    class bytes_iterator {
      public:
        explicit bytes_iterator(std::string::const_iterator it) : it_(it) {}
        ALWAYS_INLINE _int operator*() const { return static_cast<_int>(static_cast<unsigned char>(*it_)); }
        ALWAYS_INLINE bytes_iterator &operator++() {
            ++it_;
            return *this;
        }
        ALWAYS_INLINE bool operator!=(const bytes_iterator &o) const { return it_ != o.it_; }
        ALWAYS_INLINE bool operator==(const bytes_iterator &o) const { return it_ == o.it_; }

      private:
        std::string::const_iterator it_;
    };
    ALWAYS_INLINE bytes_iterator begin() const { return bytes_iterator(data_.begin()); }
    ALWAYS_INLINE bytes_iterator end() const { return bytes_iterator(data_.end()); }

  private:
    std::string data_;

    ALWAYS_INLINE static bool isAlpha(char c) { return std::isalpha((unsigned char)c) != 0; }
    ALWAYS_INLINE static bool isDigit(char c) { return std::isdigit((unsigned char)c) != 0; }
    ALWAYS_INLINE static bool isUpper(char c) { return std::isupper((unsigned char)c) != 0; }
    ALWAYS_INLINE static bool isLower(char c) { return std::islower((unsigned char)c) != 0; }
    ALWAYS_INLINE static char up(char c) { return (char)std::toupper((unsigned char)c); }
    ALWAYS_INLINE static char low(char c) { return (char)std::tolower((unsigned char)c); }

    template <typename F> bytes mapped(F f) const {
        std::string out = data_;
        for (char &c : out)
            c = (char)f((unsigned char)c);
        return bytes(std::move(out));
    }
    template <typename F> ALWAYS_INLINE bool allOf(F f) const {
        return !data_.empty() && std::all_of(data_.begin(), data_.end(), f);
    }
    bool casedAllAre(bool wantUpper) const {
        bool seen = false;
        for (char c : data_) {
            if (!isUpper(c) && !isLower(c))
                continue;
            seen = true;
            if (isUpper(c) != wantUpper)
                return false;
        }
        return seen;
    }
    ALWAYS_INLINE bytes pad(size_type width, const bytes &fill, int mode) const {
        _int extra = width - __len__();
        if (extra <= 0 || fill.data_.empty())
            return *this;
        char f = fill.data_[0];
        if (mode == 0)
            return bytes(data_ + std::string(extra, f));
        if (mode == 1)
            return bytes(std::string(extra, f) + data_);
        _int left = extra / 2;
        return bytes(std::string(left, f) + data_ + std::string(extra - left, f));
    }
    ALWAYS_INLINE static _int toIndex(size_t at) {
        return at == std::string::npos ? -1 : static_cast<_int>(at);
    }
    ALWAYS_INLINE static _int found(_int at) {
        if (at < 0)
            throw ValueError("subsection not found");
        return at;
    }
    ALWAYS_INLINE size_t clampStart(size_type start) const {
        _int n = __len__();
        if (start < 0)
            start += n;
        return static_cast<size_t>(start < 0 ? 0 : start);
    }
    ALWAYS_INLINE size_t normIndex(size_type i) const {
        _int n = __len__();
        if (i < 0)
            i += n;
        if (i < 0 || i >= n)
            throw IndexError("index out of range");
        return static_cast<size_t>(i);
    }
};

inline bytes operator*(_int n, const bytes &b) { return b * n; }
ALWAYS_INLINE _int len(const bytes &b) { return b.__len__(); }
ALWAYS_INLINE std::ostream &operator<<(std::ostream &os, const bytes &b) {
    return os << b.raw();
}

} // namespace py
