#pragma once

// Python's open() and the file object it hands back. No context manager: a
// file closes when it goes out of scope, like everything else. Reads pull
// the whole file in at once, so read/readline/readlines/iteration all just
// share one position rather than needing their own buffering.

#include "exceptions.h"
#include "list.h"
#include "str.h"
#include "types.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace py {

// Split keeping the line endings, the way Python's readlines() does: a final
// line without a newline stays without one.
inline std::vector<std::string> _split_keepends(const std::string &text) {
    std::vector<std::string> lines;
    std::string current;
    for (char c : text) {
        current += c;
        if (c == '\n') {
            lines.push_back(current);
            current.clear();
        }
    }
    if (!current.empty())
        lines.push_back(current);
    return lines;
}

class file {
  public:
    // Locals are pre-declared before their first assignment, so every held
    // type needs a default constructor - even file, which has no real
    // "empty" state; it's always overwritten before use.
    file() = default;

    file(const str &path, const str &mode) : path_(path), mode_(mode) {
        const std::string &m = mode.raw();
        writing_ = m.find('w') != std::string::npos ||
                   m.find('a') != std::string::npos;
        if (writing_) {
            auto flags = std::ios::out;
            if (m.find('a') != std::string::npos)
                flags |= std::ios::app;
            out_.open(path.raw(), flags);
            if (!out_)
                throw FileNotFoundError("cannot open " + path.raw());
            return;
        }
        std::ifstream in(path.raw());
        if (!in)
            throw FileNotFoundError("No such file or directory: '" +
                                    path.raw() + "'");
        std::ostringstream buffer;
        buffer << in.rdbuf();
        content_ = buffer.str();
    }

    ~file() { close(); }
    // An open file handle isn't copyable, only movable - like Python, where
    // there's no meaningful way to duplicate one.
    file(const file &) = delete;
    file &operator=(const file &) = delete;
    file(file &&) = default;
    file &operator=(file &&) = default;

    // The rest of the file from where the last read stopped, like Python.
    str read() {
        std::string rest = content_.substr(position_);
        position_ = content_.size();
        return str(std::move(rest));
    }

    // An empty str at end of file, again like Python.
    str readline() {
        if (position_ >= content_.size())
            return str("");
        size_t end = content_.find('\n', position_);
        size_t stop = end == std::string::npos ? content_.size() : end + 1;
        std::string line = content_.substr(position_, stop - position_);
        position_ = stop;
        return str(std::move(line));
    }

    list<str> readlines() {
        list<str> lines;
        for (const std::string &line : _split_keepends(content_.substr(position_)))
            lines.append(str(line));
        position_ = content_.size();
        return lines;
    }

    // Python returns the number of characters written.
    _int write(const str &text) {
        out_ << text.raw();
        return text.__len__();
    }

    void close() {
        if (out_.is_open())
            out_.close();
    }

    str __str__() const {
        return str("<file name='" + path_.raw() + "' mode='" + mode_.raw() +
                   "'>");
    }

    class file_iterator {
      public:
        file_iterator(file &f) : lines_(_split_keepends(f.remaining())), i_(0) {}
        bool done() const { return i_ >= lines_.size(); }
        str current() { return str(lines_[i_]); }
        str next() { return str(lines_[i_++]); }

      private:
        std::vector<std::string> lines_;
        size_t i_;
    };

    file_iterator iter() { return file_iterator(*this); }

    // Consumes what is left, so iterating twice yields nothing the second
    // time, matching a Python file object.
    std::string remaining() {
        std::string rest = content_.substr(position_);
        position_ = content_.size();
        return rest;
    }

  private:
    str path_;
    str mode_;
    bool writing_ = false;
    std::string content_;
    size_t position_ = 0;
    std::ofstream out_;
};

inline file open(const str &path, const str &mode = str("r")) {
    return file(path, mode);
}

inline auto iter(file &f) { return f.iter(); }

} // namespace py
