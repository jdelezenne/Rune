#pragma once

#include <rune/forward.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Non-owning view over a UTF-8 string.
///
/// `length` is the number of valid bytes in `data`. The view is not null-terminated.
typedef struct RuneStringSlice
{
    char const* data; ///< UTF-8 bytes, or null when empty.
    usize length;     ///< Byte length of valid data in `data`.
} RuneStringSlice;

/// Iterator over Unicode scalar values in a RuneStringSlice.
typedef struct RuneStringSliceIterator
{
    char const* data; ///< UTF-8 bytes being iterated.
    usize length;     ///< Byte length of the view.
    usize offset;     ///< Current byte offset.
} RuneStringSliceIterator;

/// Creates a non-owning slice over UTF-8 bytes.
/// @param data First byte in the view, or null for an empty slice.
/// @param length Number of bytes in the view.
/// @return String slice view.
RuneStringSlice rune_stringslice_create(char const* data, usize length);

/// Creates a non-owning slice over a byte range in `string`.
/// @param string Source string.
/// @param start Inclusive byte start.
/// @param end Exclusive byte end.
/// @return String slice view.
RuneStringSlice rune_stringslice_from_string(RuneString const* string, intptr start, intptr end);

/// Returns the byte length of `self`.
/// @param self String slice to inspect.
/// @return Byte length.
intptr rune_stringslice_byte_length(RuneStringSlice* self);

/// Returns the number of Unicode scalar values in `self`.
/// @param self String slice to inspect.
/// @return Scalar value count.
intptr rune_stringslice_count(RuneStringSlice* self);

/// Returns whether `self` contains no bytes.
/// @param self String slice to inspect.
/// @return `true` when empty.
bool rune_stringslice_is_empty(RuneStringSlice* self);

/// Returns the Unicode scalar value at scalar index `at`.
/// @param self String slice to inspect.
/// @param at Zero-based scalar index.
/// @return Code point at `at`.
RuneChar rune_stringslice_char_at(RuneStringSlice* self, intptr at);

/// Creates an iterator over a string slice's Unicode scalar values.
/// @param self Slice to iterate.
/// @return Initialised iterator.
RuneStringSliceIterator rune_stringslice_iterator_create(RuneStringSlice const* self);

/// Advances a string-slice iterator.
/// @param self Iterator to advance.
/// @param out_value Receives the next character on success.
/// @return True when a character was produced.
bool rune_stringslice_iterator_next(RuneStringSliceIterator* self, RuneChar* out_value);

#ifdef __cplusplus
}
#endif
