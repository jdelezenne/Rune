#pragma once

#include <rune/forward.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Returns the Unicode scalar value stored in `self`.
/// @param self Character storage.
/// @return Code point value.
int64 rune_char_code_point(RuneChar self);

/// Returns the number of UTF-8 bytes required to encode `self`.
/// @param self Character storage.
/// @return UTF-8 byte length (1–4).
int64 rune_char_utf8_length(RuneChar self);

/// Returns a hash value for `self`.
/// @param self Character storage.
/// @return Hash code.
int64 rune_char_hash(RuneChar self);

#ifdef __cplusplus
}
#endif
