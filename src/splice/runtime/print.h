#pragma once
#include "iter.h"
#include "list.h"
#include "range.h"
#include "str.h"
#include "tuple.h"

#include <iostream>
#include <string>

namespace py {

// print(*values, sep=" ", end="\n"). The transpiler packs *values into a
// tuple and always passes sep/end explicitly (see ArgumentTransformer), so
// there's a single call shape rather than one overload per kwargs-or-not.
template <class... Args>
void print(const tuple<Args...> &args, const str &sep, const str &end) {
    str result;
    bool first = true;

    std::apply(
        [&](const Args &...items) {
            // Fold expression: for each arg, add separator if not first, then the arg
            (([&] {
                 if (!first)
                     result += sep;
                 result += py::to_str(items);
                 first = false;
             }()),
             ...);
        },
        args.data);

    std::cout << result << end;
}

} // namespace py
