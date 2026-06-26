#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @name Utilities
/// @{

/// Combines `seed` with `value`.
/// @param seed The initial seed value.
/// @param value The value to mix in.
/// @return The combined hash value.
uint64 rune_hash_combine(uint64 seed, uint64 value);

/// Hashes a byte range.
/// @param data Pointer to the byte data.
/// @param length Number of bytes to hash.
/// @param seed Initial seed value.
/// @return The computed hash value.
uint64 rune_hash_bytes(void const* data, usize length, uint64 seed);

/// Hashes a byte range as a string.
/// @param data Pointer to the string data.
/// @param length Number of bytes to hash.
/// @param seed Initial seed value.
/// @return The computed hash value.
uint64 rune_hash_string(char const* data, usize length, uint64 seed);

/// Hashes a null-terminated C string.
/// @param data Pointer to the null-terminated string.
/// @param seed Initial seed value.
/// @return The computed hash value.
uint64 rune_hash_cstring(char const* data, uint64 seed);

/// @}

/// @name Primitive Types
/// @{

/// Returns a hash value for `bool`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_bool_hash(bool value);

/// Returns a hash value for `int8`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_int8_hash(int8 value);

/// Returns a hash value for `int16`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_int16_hash(int16 value);

/// Returns a hash value for `int32`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_int32_hash(int32 value);

/// Returns a hash value for `int64`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_int64_hash(int64 value);

/// Returns a hash value for `intptr`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_int_hash(intptr value);

/// Returns a hash value for `uint8`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_uint8_hash(uint8 value);

/// Returns a hash value for `uint16`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_uint16_hash(uint16 value);

/// Returns a hash value for `uint32`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_uint32_hash(uint32 value);

/// Returns a hash value for `uint64`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_uint64_hash(uint64 value);

/// Returns a hash value for `uintptr`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_uint_hash(uintptr value);

/// Returns a hash value for `float32`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_float32_hash(float32 value);

/// Returns a hash value for `float64`.
/// @param value Value to hash.
/// @return The hash value.
int64 rune_float64_hash(float64 value);

/// @}

#ifdef __cplusplus
}
#endif
