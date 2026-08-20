#pragma once

// Force inline attribute
#if defined(__GNUC__) || defined(__clang__)
#define ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif

// Restrict hint used on all parameters
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

// Flatten hint to be used on hotpath decorated functions
#if defined(__GNUC__) || defined(__clang__)
#define FLATTEN __attribute__((flatten))
#else
#define FLATTEN
#endif
