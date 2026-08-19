#pragma once
#include "iter.h"
#include "list.h"
#include "range.h"
#include "str.h"
#include "tuple.h"

#include <iostream>
#include <string>

namespace py {

// Internal implementation - takes sep, end, and variadic args explicitly
// Called by transpiler when kwargs are present
template <class... Args>
void _print_kwargs(const str &sep, const str &end,
                   Args... args) {
    str result;
    bool first = true;

    // Fold expression: for each arg, add separator if not first, then the arg
    (([&] {
         if (!first)
             result += sep;
         result += py::to_str(args);
         first = false;
     }()),
     ...);

    std::cout << result << end;
}

// Overload for no arguments with kwargs
void _print_kwargs(const str &sep, const str &end) {
    std::cout << end;
}

// Public API - no kwargs, uses default sep and end
template <class... Args> void print(Args... args) {
    _print_kwargs(" ", "\n", args...);
}

// Public API - no arguments
void print() { _print_kwargs(" ", "\n"); }

} // namespace py
