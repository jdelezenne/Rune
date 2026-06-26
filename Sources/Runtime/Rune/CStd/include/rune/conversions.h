#pragma once

#include <rune/cstring.h>
#include <rune/string.h>
#include <rune/stringslice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @name String to scalar
/// @{

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as int64.
int64 rune_string_to_int64(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as pointer-width int.
intptr rune_string_to_int(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as int8.
int8 rune_string_to_int8(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as int16.
int16 rune_string_to_int16(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as int32.
int32 rune_string_to_int32(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as uint64.
uint64 rune_string_to_uint64(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as pointer-width uint.
uintptr rune_string_to_uint(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as uint8.
uint8 rune_string_to_uint8(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as uint16.
uint16 rune_string_to_uint16(RuneString const* value);

/// Parses `value` as a decimal integer.
/// @param value String to parse.
/// @return Parsed value as uint32.
uint32 rune_string_to_uint32(RuneString const* value);

/// Parses `value` as a floating-point number.
/// @param value String to parse.
/// @return Parsed value as float32.
float32 rune_string_to_float32(RuneString const* value);

/// Parses `value` as a floating-point number.
/// @param value String to parse.
/// @return Parsed value as float64.
float64 rune_string_to_float64(RuneString const* value);

/// Parses `value` as a boolean (`"true"` or `"false"`).
/// @param value String to parse.
/// @return Parsed boolean value.
bool rune_string_to_bool(RuneString const* value);

/// @}

/// @name Checked primitive conversions
/// @{

/// Converts `value` to an integer, trapping on overflow or non-finite input.
/// @param value Float to convert.
/// @return Truncated integer value.
intptr rune_float32_to_int_checked(float32 value);

/// Converts `value` to an integer, trapping on overflow or non-finite input.
/// @param value Float to convert.
/// @return Truncated integer value.
intptr rune_float64_to_int_checked(float64 value);

/// @}

/// @name String initializer backing symbols
/// @{

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_int8_from_string(int8* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_int16_from_string(int16* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_int32_from_string(int32* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_int64_from_string(int64* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_int_from_string(intptr* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_uint8_from_string(uint8* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_uint16_from_string(uint16* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_uint32_from_string(uint32* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_uint64_from_string(uint64* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_uint_from_string(uintptr* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_float32_from_string(float32* self, RuneString* value);

/// Initialises `*self` from the string representation in `value`.
/// @param self Destination scalar storage.
/// @param value Source string.
void rune_float64_from_string(float64* self, RuneString* value);

/// @}

#ifdef __cplusplus
}
#endif
