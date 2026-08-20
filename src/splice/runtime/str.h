#pragma once

// Python str on top of std::string. ASCII only: len() and indexing count
// bytes, not code points, so non-ASCII text will not match Python.
//
// `str` is the type, so the conversion is to_str(), matching to_int/
// to_float/to_bool. Methods returning list<str> (split, ...) are declared
// here and defined in strops.h, since list.h includes this header.

#include "exceptions.h"
#include "truthy.h"
#include "types.h"
#include <type_traits>
#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <cmath>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace py {

template <typename T> class list;
template <typename... Ts> class tuple;
class slice;

class str {
  public:
    using size_type = _int;

    str() = default;
    str(const char *s) : data_(s) {}
    str(const std::string &s) : data_(s) {}
    str(std::string &&s) : data_(std::move(s)) {}

    const std::string &raw() const noexcept { return data_; }

    str __str__() const { return *this; }
    // Containers render elements with repr(), so they show 'a' rather than a.
    str __repr__() const { return str("'" + data_ + "'"); }

    size_type __len__() const noexcept {
        return static_cast<size_type>(data_.size());
    }
    size_type len() const noexcept { return __len__(); }
    bool empty() const noexcept { return data_.empty(); }
    explicit operator bool() const noexcept { return !data_.empty(); }
    size_t __hash__() const { return std::hash<std::string>{}(data_); }

    // Indexing yields a length-1 str: Python has no separate char type.
    str __getitem__(size_type i) const {
        return str(std::string(1, data_[normIndex(i)]));
    }
    // s[-1] -- a length-1 str, same as __getitem__.
    str back() const {
        if (data_.empty())
            throw IndexError("string index out of range");
        return str(std::string(1, data_.back()));
    }
    // s[i:j:k] -- defined in slice.h, which this header cannot include.
    str __getitem__(const slice &s) const;
    bool __contains__(const str &sub) const {
        return data_.find(sub.data_) != std::string::npos;
    }

    str upper() const { return mapped(::toupper); }
    str lower() const { return mapped(::tolower); }
    str swapcase() const {
        std::string out = data_;
        for (char &c : out)
            c = isLower(c) ? up(c) : (isUpper(c) ? low(c) : c);
        return str(std::move(out));
    }
    str capitalize() const {
        std::string out = lower().data_;
        if (!out.empty())
            out[0] = up(out[0]);
        return str(std::move(out));
    }
    str title() const {
        std::string out = data_;
        bool start = true;
        for (char &c : out) {
            c = start ? up(c) : low(c);
            start = !isAlpha(c);
        }
        return str(std::move(out));
    }
    str casefold() const { return lower(); }

    // find returns -1 when absent; index raises, which is the only difference.
    _int find(const str &sub) const { return toIndex(data_.find(sub.data_)); }
    _int find(const str &sub, size_type start) const {
        return toIndex(data_.find(sub.data_, clampStart(start)));
    }
    _int rfind(const str &sub) const { return toIndex(data_.rfind(sub.data_)); }
    _int index(const str &sub) const { return found(find(sub)); }
    _int rindex(const str &sub) const { return found(rfind(sub)); }

    bool startswith(const str &prefix) const {
        return data_.rfind(prefix.data_, 0) == 0;
    }
    bool endswith(const str &suffix) const {
        return data_.size() >= suffix.data_.size() &&
               data_.compare(data_.size() - suffix.data_.size(),
                             suffix.data_.size(), suffix.data_) == 0;
    }

    _int count(const str &sub) const {
        if (sub.data_.empty())
            return __len__() + 1;
        _int n = 0;
        for (size_t at = data_.find(sub.data_); at != std::string::npos;
             at = data_.find(sub.data_, at + sub.data_.size()))
            ++n;
        return n;
    }

    str replace(const str &old_s, const str &new_s, _int count = -1) const {
        if (old_s.data_.empty())
            return *this;
        std::string out;
        size_t at = 0, prev = 0;
        while (count != 0 && (at = data_.find(old_s.data_, prev)) !=
                                 std::string::npos) {
            out.append(data_, prev, at - prev).append(new_s.data_);
            prev = at + old_s.data_.size();
            if (count > 0)
                --count;
        }
        out.append(data_, prev, std::string::npos);
        return str(std::move(out));
    }

    str removeprefix(const str &prefix) const {
        return startswith(prefix) ? str(data_.substr(prefix.data_.size()))
                                  : *this;
    }
    str removesuffix(const str &suffix) const {
        return !suffix.data_.empty() && endswith(suffix)
                   ? str(data_.substr(0, data_.size() - suffix.data_.size()))
                   : *this;
    }

    // No argument strips whitespace, like Python.
    str strip() const { return lstrip().rstrip(); }
    str lstrip() const {
        size_t b = data_.find_first_not_of(" \t\n\r\f\v");
        return str(b == std::string::npos ? "" : data_.substr(b));
    }
    str rstrip() const {
        size_t e = data_.find_last_not_of(" \t\n\r\f\v");
        return str(e == std::string::npos ? "" : data_.substr(0, e + 1));
    }
    str strip(const str &chars) const { return lstrip(chars).rstrip(chars); }
    str lstrip(const str &chars) const {
        size_t b = data_.find_first_not_of(chars.data_);
        return str(b == std::string::npos ? "" : data_.substr(b));
    }
    str rstrip(const str &chars) const {
        size_t e = data_.find_last_not_of(chars.data_);
        return str(e == std::string::npos ? "" : data_.substr(0, e + 1));
    }

    str ljust(size_type width, const str &fill = " ") const {
        return pad(width, fill, 0);
    }
    str rjust(size_type width, const str &fill = " ") const {
        return pad(width, fill, 1);
    }
    str center(size_type width, const str &fill = " ") const {
        return pad(width, fill, 2);
    }
    str zfill(size_type width) const {
        _int extra = width - __len__();
        if (extra <= 0)
            return *this;
        bool signed_ = !data_.empty() && (data_[0] == '-' || data_[0] == '+');
        std::string out = signed_ ? std::string(1, data_[0]) : std::string();
        out += std::string(extra, '0');
        out += signed_ ? data_.substr(1) : data_;
        return str(std::move(out));
    }

    // All are false for the empty string, like Python.
    bool isalpha() const { return allOf(isAlpha); }
    bool isdigit() const { return allOf(isDigit); }
    bool isalnum() const {
        return allOf([](char c) { return isAlpha(c) || isDigit(c); });
    }
    bool isspace() const {
        return allOf([](char c) { return std::isspace((unsigned char)c) != 0; });
    }
    bool isdecimal() const { return isdigit(); }
    bool isnumeric() const { return isdigit(); }
    bool isascii() const {
        return std::all_of(data_.begin(), data_.end(),
                           [](char c) { return (unsigned char)c < 128; });
    }
    // Cased characters must agree, and there must be at least one.
    bool isupper() const { return casedAllAre(true); }
    bool islower() const { return casedAllAre(false); }

    // list/tuple-returning: defined in strops.h.
    list<str> split() const;                  // on whitespace
    list<str> split(const str &sep) const;
    list<str> rsplit(const str &sep) const;
    list<str> splitlines() const;
    str join(const list<str> &parts) const;
    tuple<str, str, str> partition(const str &sep) const;
    tuple<str, str, str> rpartition(const str &sep) const;

    // format: PEP 3101 Format Mini-Language, enough for str.format()
    // and the "{conv:{}}" calls f-strings desugar into. Defined below, after
    // to_str()/repr(), which it needs to render arbitrary argument types.
    template <typename... Args> str format(const Args &...args) const;

    str operator+(const str &o) const { return str(data_ + o.data_); }
    str &operator+=(const str &o) {
        data_ += o.data_;
        return *this;
    }
    str operator*(_int n) const {
        std::string out;
        for (_int k = 0; k < n; ++k)
            out += data_;
        return str(std::move(out));
    }
    bool operator==(const str &o) const { return data_ == o.data_; }
    bool operator!=(const str &o) const { return data_ != o.data_; }
    bool operator<(const str &o) const { return data_ < o.data_; }
    bool operator<=(const str &o) const { return data_ <= o.data_; }
    bool operator>(const str &o) const { return data_ > o.data_; }
    bool operator>=(const str &o) const { return data_ >= o.data_; }

    // Iterating a string yields its characters as length-1 strs.
    class str_iterator {
      public:
        explicit str_iterator(std::string::const_iterator it) : it_(it) {}
        str operator*() const { return str(std::string(1, *it_)); }
        str_iterator &operator++() {
            ++it_;
            return *this;
        }
        bool operator!=(const str_iterator &o) const { return it_ != o.it_; }
        bool operator==(const str_iterator &o) const { return it_ == o.it_; }

      private:
        std::string::const_iterator it_;
    };
    str_iterator begin() const { return str_iterator(data_.begin()); }
    str_iterator end() const { return str_iterator(data_.end()); }

  private:
    std::string data_;

    static bool isAlpha(char c) { return std::isalpha((unsigned char)c) != 0; }
    static bool isDigit(char c) { return std::isdigit((unsigned char)c) != 0; }
    static bool isUpper(char c) { return std::isupper((unsigned char)c) != 0; }
    static bool isLower(char c) { return std::islower((unsigned char)c) != 0; }
    static char up(char c) { return (char)std::toupper((unsigned char)c); }
    static char low(char c) { return (char)std::tolower((unsigned char)c); }

    template <typename F> str mapped(F f) const {
        std::string out = data_;
        for (char &c : out)
            c = (char)f((unsigned char)c);
        return str(std::move(out));
    }
    template <typename F> bool allOf(F f) const {
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
    str pad(size_type width, const str &fill, int mode) const {
        _int extra = width - __len__();
        if (extra <= 0 || fill.data_.empty())
            return *this;
        char f = fill.data_[0];
        if (mode == 0)
            return str(data_ + std::string(extra, f));
        if (mode == 1)
            return str(std::string(extra, f) + data_);
        _int left = extra / 2;
        return str(std::string(left, f) + data_ + std::string(extra - left, f));
    }
    static _int toIndex(size_t at) {
        return at == std::string::npos ? -1 : static_cast<_int>(at);
    }
    static _int found(_int at) {
        if (at < 0)
            throw ValueError("substring not found");
        return at;
    }
    size_t clampStart(size_type start) const {
        _int n = __len__();
        if (start < 0)
            start += n;
        return static_cast<size_t>(start < 0 ? 0 : start);
    }
    size_t normIndex(size_type i) const {
        _int n = __len__();
        if (i < 0)
            i += n;
        if (i < 0 || i >= n)
            throw IndexError("string index out of range");
        return static_cast<size_t>(i);
    }
};

inline str operator*(_int n, const str &s) { return s * n; }
inline _int len(const str &s) { return s.__len__(); }

// Anything else that defines __len__, so a user's class gets len() the same
// way the containers do. The per container overloads are more specialised and
// still win for those.
template <class T, class = std::enable_if_t<detail::has_len_method<T>::value>>
inline _int len(const T &x) {
    return const_cast<T &>(x).__len__();
}
inline str operator+(const char *a, const str &b) { return str(a) + b; }
inline std::ostream &operator<<(std::ostream &os, const str &s) {
    return os << s.raw();
}

inline str to_str(_int x) { return str(std::to_string(x)); }

inline str to_str(_float x) {
    std::array<char, 32> buf;
    auto result = std::to_chars(buf.data(), buf.data() + buf.size(), x);
    std::string s(buf.data(), result.ptr);
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos &&
        s.find("inf") == std::string::npos &&
        s.find("nan") == std::string::npos) {
        s += ".0";
    }
    return str(std::move(s));
}

inline str to_str(bool x) { return str(x ? "True" : "False"); }
inline str to_str(const char *s) { return str(s); }
inline str to_str(const std::string &s) { return str(s); }

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
inline str to_str(T x) {
    return to_str(static_cast<_int>(x));
}

inline str PyException::__str__() const { return str(what()); }

// Everything else is a class of ours, and renders itself: to_str() is the
// free spelling of __str__(). Detection and the call both go through a
// mutable reference because transpiled methods aren't const, matching
// Python; casting the const away is safe since __str__ never mutates.
template <class T, class = void> struct has_str : std::false_type {};
template <class T>
struct has_str<T, std::void_t<decltype(std::declval<T &>().__str__())>>
    : std::true_type {};

template <class T, class = void> struct has_repr : std::false_type {};
template <class T>
struct has_repr<T, std::void_t<decltype(std::declval<T &>().__repr__())>>
    : std::true_type {};

template <class T, class = std::enable_if_t<has_str<T>::value>>
str to_str(const T &x) {
    return const_cast<T &>(x).__str__();
}

// repr() - like to_str(), but quotes strings. Containers render their
// elements with repr(), so Python shows {'a': 1} not {a: 1}. Only a class
// that has to differ from its __str__ needs its own __repr__.
inline str repr(const char *s) { return str("'" + std::string(s) + "'"); }

template <typename T> str repr(const T &x) {
    if constexpr (has_repr<T>::value) {
        return const_cast<T &>(x).__repr__();
    } else {
        return to_str(x);
    }
}

// A parsed "[[fill]align][sign][#][0][width][,][.precision][type]" spec, per
// the Format Mini-Language (PEP 3101).
namespace detail {

struct FormatSpec {
    char fill = ' ';
    char align = '\0'; // '<', '>', '^', '=', or '\0' for the type's default
    char sign = '-';   // '+', '-', ' '
    bool alt = false;  // '#'
    _int width = 0;
    bool group = false;  // ',' or '_' thousands separator
    _int precision = -1; // -1 means unset
    char type = '\0';
};

inline FormatSpec parse_format_spec(const std::string &s) {
    FormatSpec spec;
    size_t i = 0;
    auto is_align = [](char c) {
        return c == '<' || c == '>' || c == '^' || c == '=';
    };
    if (s.size() >= 2 && is_align(s[1])) {
        spec.fill = s[0];
        spec.align = s[1];
        i = 2;
    } else if (!s.empty() && is_align(s[0])) {
        spec.align = s[0];
        i = 1;
    }
    if (i < s.size() && (s[i] == '+' || s[i] == '-' || s[i] == ' ')) {
        spec.sign = s[i];
        ++i;
    }
    if (i < s.size() && s[i] == '#') {
        spec.alt = true;
        ++i;
    }
    if (i < s.size() && s[i] == '0') {
        if (spec.align == '\0') {
            spec.align = '=';
            spec.fill = '0';
        }
        ++i;
    }
    size_t wstart = i;
    while (i < s.size() && std::isdigit((unsigned char)s[i]))
        ++i;
    if (i > wstart)
        spec.width = std::stoll(s.substr(wstart, i - wstart));
    if (i < s.size() && (s[i] == ',' || s[i] == '_')) {
        spec.group = true;
        ++i;
    }
    if (i < s.size() && s[i] == '.') {
        ++i;
        size_t pstart = i;
        while (i < s.size() && std::isdigit((unsigned char)s[i]))
            ++i;
        spec.precision = std::stoll(s.substr(pstart, i - pstart));
    }
    if (i < s.size())
        spec.type = s[i];
    return spec;
}

// Pads `body` to spec.width. `default_align` is the type's own default
// ('>' for numbers, '<' for strings) when the spec doesn't set one.
inline std::string pad(const std::string &body, const FormatSpec &spec,
                       char default_align) {
    _int extra = spec.width - static_cast<_int>(body.size());
    if (extra <= 0)
        return body;
    char align = spec.align ? spec.align : default_align;
    if (align == '<')
        return body + std::string(extra, spec.fill);
    if (align == '^') {
        _int left = extra / 2;
        return std::string(left, spec.fill) + body + std::string(extra - left, spec.fill);
    }
    if (align == '=') {
        // Padding goes after a leading sign, so "-0005" not "000-5".
        bool has_sign = !body.empty() && (body[0] == '-' || body[0] == '+' || body[0] == ' ');
        return has_sign ? body.substr(0, 1) + std::string(extra, spec.fill) + body.substr(1)
                        : std::string(extra, spec.fill) + body;
    }
    return std::string(extra, spec.fill) + body; // '>'
}

inline std::string signed_prefix(bool negative, char sign_mode) {
    if (negative)
        return "-";
    if (sign_mode == '+')
        return "+";
    if (sign_mode == ' ')
        return " ";
    return "";
}

inline void add_thousands(std::string &digits) {
    for (_int pos = static_cast<_int>(digits.size()) - 3; pos > 0; pos -= 3)
        digits.insert(static_cast<size_t>(pos), ",");
}

inline std::string apply_format(const FormatSpec &spec, _int value) {
    bool negative = value < 0;
    // Cast first, then negate in unsigned arithmetic: negating a signed
    // INT64_MIN directly overflows, but this wraps correctly.
    unsigned long long mag =
        negative ? (0ULL - (unsigned long long)value) : (unsigned long long)value;
    std::string digits, prefix;
    int base = 10;
    switch (spec.type) {
    case 'x': base = 16; prefix = spec.alt ? "0x" : ""; break;
    case 'X': base = 16; prefix = spec.alt ? "0X" : ""; break;
    case 'o': base = 8; prefix = spec.alt ? "0o" : ""; break;
    case 'b': base = 2; prefix = spec.alt ? "0b" : ""; break;
    default: break;
    }
    if (base == 10) {
        digits = std::to_string(mag);
        if (spec.group)
            add_thousands(digits);
    } else if (mag == 0) {
        digits = "0";
    } else {
        const char *hexd = spec.type == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
        while (mag) {
            digits.insert(digits.begin(), hexd[mag % (unsigned)base]);
            mag /= (unsigned)base;
        }
    }
    return pad(signed_prefix(negative, spec.sign) + prefix + digits, spec, '>');
}

inline std::string apply_format(const FormatSpec &spec, _float value) {
    if (!spec.type) {
        // No type char: match to_str()'s shortest round-trip rendering,
        // which is Python's own default for a float with an empty spec.
        std::string body = signed_prefix(value < 0, spec.sign) +
                           (value < 0 ? to_str(-value).raw() : to_str(value).raw());
        return pad(body, spec, '>');
    }
    bool negative = std::signbit(value);
    _float mag = negative ? -value : value;
    _int precision = spec.precision < 0 ? 6 : spec.precision;
    if (spec.type == '%')
        mag *= 100;
    auto fmt = (spec.type == 'e' || spec.type == 'E') ? std::chars_format::scientific
              : (spec.type == 'g' || spec.type == 'G') ? std::chars_format::general
                                                        : std::chars_format::fixed;
    std::array<char, 64> buf;
    auto result =
        std::to_chars(buf.data(), buf.data() + buf.size(), mag, fmt, (int)precision);
    std::string digits(buf.data(), result.ptr);
    if (spec.type == '%')
        digits += "%";
    return pad(signed_prefix(negative, spec.sign) + digits, spec, '>');
}

inline std::string apply_format(const FormatSpec &spec, bool value) {
    if (spec.type)
        return apply_format(spec, static_cast<_int>(value));
    return pad(value ? "True" : "False", spec, '<');
}

inline std::string apply_format(const FormatSpec &spec, const std::string &value) {
    std::string body = spec.precision >= 0 && spec.precision < (_int)value.size()
                           ? value.substr(0, (size_t)spec.precision)
                           : value;
    return pad(body, spec, '<');
}

inline std::string apply_format(const FormatSpec &spec, const str &value) {
    return apply_format(spec, value.raw());
}

// Fallback for any other type: render via to_str(), like Python falls back
// to object.__format__, which calls str().
template <typename T>
std::string apply_format(const FormatSpec &spec, const T &value) {
    return apply_format(spec, to_str(value).raw());
}

template <typename T> std::string render_arg(const T &value, const std::string &raw_spec) {
    return apply_format(parse_format_spec(raw_spec), value);
}

// One entry per positional argument to format(): a spec-aware renderer plus
// str()/repr() renderers for the "!s"/"!r"/"!a" conversions, which convert
// to a string *before* the (string) spec is applied.
struct FormatArg {
    std::function<std::string(const std::string &)> typed;
    std::function<std::string()> as_str;
    std::function<std::string()> as_repr;
};

class FormatArgs {
  public:
    template <typename... Ts> explicit FormatArgs(const Ts &...values) {
        (args_.push_back(FormatArg{
             [&values](const std::string &spec) { return render_arg(values, spec); },
             [&values] { return to_str(values).raw(); },
             [&values] { return repr(values).raw(); },
         }),
         ...);
    }

    std::string render(size_t index, char conv, const std::string &spec) const {
        if (index >= args_.size())
            throw IndexError("Replacement index out of range for positional args tuple");
        const FormatArg &a = args_[index];
        if (conv == 'r')
            return apply_format(parse_format_spec(spec), a.as_repr());
        if (conv == 's' || conv == 'a')
            return apply_format(parse_format_spec(spec), a.as_str());
        return a.typed(spec);
    }

  private:
    std::vector<FormatArg> args_;
};

// Finds the '}' matching the '{' at tmpl[open], honoring nesting so a spec
// like "{:{}}" doesn't stop at the inner field's closing brace.
inline size_t find_matching_brace(const std::string &tmpl, size_t open) {
    size_t depth = 1;
    size_t j = open + 1;
    for (; j < tmpl.size() && depth > 0; ++j) {
        if (tmpl[j] == '{')
            ++depth;
        else if (tmpl[j] == '}')
            --depth;
    }
    if (depth != 0)
        throw ValueError("Single '{' encountered in format string");
    return j - 1;
}

// Resolves one level of nested replacement fields inside a spec, e.g. the
// "{}" in "{:{}}" -- the shape f-strings desugar their format specs into.
inline std::string resolve_nested_spec(const std::string &raw_spec, size_t &auto_index,
                                       const FormatArgs &args) {
    std::string spec;
    size_t j = 0;
    while (j < raw_spec.size()) {
        if (raw_spec[j] != '{') {
            spec += raw_spec[j++];
            continue;
        }
        size_t end = find_matching_brace(raw_spec, j);
        std::string field = raw_spec.substr(j + 1, end - j - 1);
        size_t index = field.empty() ? auto_index++ : (size_t)std::stoull(field);
        spec += args.render(index, '\0', "");
        j = end + 1;
    }
    return spec;
}

inline std::string format_impl(const std::string &tmpl, const FormatArgs &args) {
    std::string out;
    size_t auto_index = 0;
    size_t i = 0;
    while (i < tmpl.size()) {
        if (tmpl[i] == '{' && i + 1 < tmpl.size() && tmpl[i + 1] == '{') {
            out += '{';
            i += 2;
            continue;
        }
        if (tmpl[i] == '}' && i + 1 < tmpl.size() && tmpl[i + 1] == '}') {
            out += '}';
            i += 2;
            continue;
        }
        if (tmpl[i] != '{') {
            out += tmpl[i++];
            continue;
        }
        size_t end = find_matching_brace(tmpl, i);
        std::string field = tmpl.substr(i + 1, end - i - 1);
        i = end + 1;

        // field := [index] ['!' conv] [':' spec]
        size_t colon = field.find(':');
        std::string head = colon == std::string::npos ? field : field.substr(0, colon);
        std::string raw_spec = colon == std::string::npos ? "" : field.substr(colon + 1);

        char conv = '\0';
        size_t bang = head.find('!');
        if (bang != std::string::npos) {
            conv = head[bang + 1];
            head = head.substr(0, bang);
        }

        size_t index = head.empty() ? auto_index++ : (size_t)std::stoull(head);
        std::string spec = resolve_nested_spec(raw_spec, auto_index, args);
        out += args.render(index, conv, spec);
    }
    return out;
}

} // namespace detail

template <typename... Args> str str::format(const Args &...args) const {
    detail::FormatArgs fa(args...);
    return str(detail::format_impl(data_, fa));
}

} // namespace py
