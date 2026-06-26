#pragma once

#include <rune/forward.h>
#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Returns the Unicode scalar value stored in `self`.
/// @param self Character storage.
/// @return Code point value as a pointer-sized integer (matches Rune `Int`).
intptr rune_char_code_point(RuneChar self);

/// Returns the number of UTF-8 bytes required to encode `self`.
/// @param self Character storage.
/// @return UTF-8 byte length (1–4) as a pointer-sized integer (matches Rune `Int`).
intptr rune_char_utf8_length(RuneChar self);

/// Returns a hash value for `self`.
/// @param self Character storage.
/// @return Hash code.
int64 rune_char_hash(RuneChar self);

/// Encodes `self` as a one-character UTF-8 string.
/// @param allocator Allocator used for heap storage.
/// @param self Character storage.
/// @return A string containing `self`.
RuneString rune_string_from_char(RuneAllocator const* allocator, RuneChar self);

#ifdef __cplusplus
}
#endif
