#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Marks a parameter or variable as intentionally unused, suppressing compiler warnings.
#define RUNE_UNUSED(x) ((void)(x))

/// Maximum natural alignment guaranteed by the system allocator.
/// Safe to use with `alignas` when the required alignment is not known at compile time.
#define RUNE_MAX_ALIGN (sizeof(void*) > sizeof(double) ? sizeof(void*) : sizeof(double))

/// @name Signed Integer Types
/// @{
typedef int8_t int8;   ///< 8-bit signed integer.
typedef int16_t int16; ///< 16-bit signed integer.
typedef int32_t int32; ///< 32-bit signed integer.
typedef int64_t int64; ///< 64-bit signed integer.
/// @}

/// @name Unsigned Integer Types
/// @{
typedef uint8_t uint8;   ///< 8-bit unsigned integer.
typedef uint16_t uint16; ///< 16-bit unsigned integer.
typedef uint32_t uint32; ///< 32-bit unsigned integer.
typedef uint64_t uint64; ///< 64-bit unsigned integer.

typedef uint32 RuneChar; ///< Unicode scalar value used by Rune `Char`.
/// @}

/// Raw byte type.
typedef uint8_t byte;

/// @name Size and Pointer Types
/// @{
typedef intptr_t intptr;   ///< Signed pointer-sized integer.
typedef uintptr_t uintptr; ///< Unsigned pointer-sized integer.
typedef ptrdiff_t isize;   ///< Signed size difference type.
typedef size_t usize;      ///< Unsigned size type.
/// @}

/// @name Floating-Point Types
/// @{
typedef float float32;        ///< 32-bit floating-point type.
typedef double float64;       ///< 64-bit floating-point type.
typedef long double float128; ///< 128-bit floating-point type.
/// @}

/// @name Collection Callback Types
/// @{

/// Hashes `key` to a 64-bit value. Null signals the default byte-wise FNV hash.
typedef uint64 (*RuneHashFn)(void const* key);

/// Returns `true` when `a` and `b` are equal. Null signals the default byte-wise comparison.
typedef bool (*RuneEqualFn)(void const* a, void const* b);

/// Returns negative, zero, or positive as `a` compares less-than, equal-to, or
/// greater-than `b`. Null signals the default byte-wise comparison.
typedef int (*RuneCompareFn)(void const* a, void const* b);

/// @}

#ifdef __cplusplus
}
#endif
