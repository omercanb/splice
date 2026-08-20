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
