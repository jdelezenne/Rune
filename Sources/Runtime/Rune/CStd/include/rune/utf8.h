#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Unicode scalar value type used by UTF-8 helpers.
typedef RuneChar RuneCodePoint;

/// Returns the number of Unicode scalar values encoded in `bytes`.
/// @param bytes UTF-8 byte pointer.
/// @param byte_length Number of bytes to inspect.
/// @return Scalar value count.
intptr rune_utf8_codepoint_count(uint8 const* bytes, usize byte_length);

/// Reads the scalar value at scalar index `scalar_index`.
/// @param bytes UTF-8 byte pointer.
/// @param byte_length Number of bytes available.
/// @param scalar_index Zero-based scalar index.
/// @param out_codepoint Receives the decoded code point on success.
/// @return `true` when the index is in range and decoding succeeds.
bool rune_utf8_codepoint_at_scalar_index(
    uint8 const* bytes,
    usize byte_length,
    intptr scalar_index,
    RuneCodePoint* out_codepoint);

/// Returns the byte offset of scalar index `scalar_index`.
/// @param bytes UTF-8 byte pointer.
/// @param byte_length Number of bytes available.
/// @param scalar_index Zero-based scalar index.
/// @param out_byte_offset Receives the byte offset on success.
/// @return `true` when the index is in range.
bool rune_utf8_byte_offset_at_scalar_index(
    uint8 const* bytes,
    usize byte_length,
    intptr scalar_index,
    usize* out_byte_offset);

/// Encodes `codepoint` into `output`.
/// @param codepoint Unicode scalar value to encode.
/// @param output Destination buffer large enough for the encoded sequence.
/// @return Number of bytes written to `output`.
uint8 rune_utf8_encode(RuneCodePoint codepoint, uint8* output);

#ifdef __cplusplus
}
#endif
