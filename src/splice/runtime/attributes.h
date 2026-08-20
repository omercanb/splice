#pragma once

// Portable force-inline. Plain `inline` is only a linkage hint - compilers
// routinely ignore it for their own cost-based heuristics. Unnamespaced, like
// types.h, so generated code and other unnamespaced headers can use it.
#if defined(__GNUC__) || defined(__clang__)
#define ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif

// Non-aliasing hint. `restrict` itself is C99, not valid C++; `__restrict` is
// the one spelling GCC, Clang and MSVC all accept (GCC/Clang also take
// `__restrict__`, MSVC doesn't). Safe to no-op on an unrecognized compiler -
// unlike ALWAYS_INLINE, this is only ever an optimization hint.
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif
