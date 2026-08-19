#pragma once

#include <cstdint>

// The C++ types representing Python's `int` and `float` - change here to
// retarget the whole runtime. Global namespace, not py::, so unnamespaced
// headers and generated code can use them unqualified.
using _int = int64_t;
using _float = double;
