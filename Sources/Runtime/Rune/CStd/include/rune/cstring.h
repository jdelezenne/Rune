#pragma once

#include <rune/allocator.h>
#include <rune/forward.h>
#include <rune/memory.h>
#include <rune/string.h>
#include <rune/stringslice.h>

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Owned, null-terminated UTF-8 string.
///
/// Always owns its heap storage. `byte_length` excludes the null terminator.
typedef struct RuneCString
{
    RuneAllocator const* allocator; ///< Allocator used for heap storage.
    char* data;                     ///< Null-terminated UTF-8 bytes, or null when empty.
    usize byte_length;              ///< Byte length excluding the null terminator.
} RuneCString;

/// Returns an empty C string bound to `allocator`.
/// @param allocator Allocator used for heap storage.
/// @return Initialised empty C string.
RuneCString rune_cstring_create(RuneAllocator const* allocator);

/// Releases the backing allocation and zeroes the C string fields.
/// @param storage C string storage to tear down.
void rune_cstring_destroy(RuneCString* storage);

/// Returns a pointer to the null-terminated UTF-8 bytes of `cstring`.
/// @param cstring C string to inspect.
/// @return Read-only null-terminated bytes, or null when empty.
char const* rune_cstring_data(RuneCString const* cstring);

/// Returns the length in bytes of a null-terminated byte string.
/// @param data Null-terminated bytes. Null yields 0.
/// @return Number of bytes before the null terminator.
usize rune_cstring_length(char const* data);

/// Lexicographically compares two null-terminated byte strings.
/// @param left Left-hand string. Null sorts before non-null.
/// @param right Right-hand string. Null sorts before non-null.
/// @return `-1` (`Ordering.Less`), `0` (`Ordering.Equal`), or `+1` (`Ordering.Greater`).
int8_t rune_cstring_compare(char const* left, char const* right);

/// Returns whether two null-terminated byte strings have identical contents.
/// @param left Left-hand string.
/// @param right Right-hand string.
/// @return True when both strings are equal.
bool rune_cstring_equal(char const* left, char const* right);

/// Replaces `cstring` with a copy of the null-terminated `data`.
/// @param cstring Target C string.
/// @param data Source null-terminated UTF-8 bytes.
void rune_cstring_assign_data(RuneCString* cstring, char const* data);

/// Deep-copies `source` into `destination`.
/// @param destination Destination C string storage.
/// @param source Source C string storage.
void rune_cstring_copy(RuneCString* destination, RuneCString const* source);

/// Replaces `self` with a null-terminated copy of `value`.
/// @param self Target C string.
/// @param value Source string.
void rune_cstring_from_string(RuneCString* self, RuneString* value);

/// Replaces `self` with a null-terminated copy of `value`.
/// @param self Target C string.
/// @param value Source string slice.
void rune_cstring_from_string_slice(RuneCString* self, RuneStringSlice* value);

/// Returns a non-owning view over the UTF-8 bytes of `self`. Zero-copy.
/// @param out Output slice. Set to `{null, 0}` when `self` is empty.
/// @param self Source C string.
void rune_cstring_as_string_slice(RuneStringSlice* out, RuneCString const* self);

/// Returns a borrowed C string slice over `cstring`.
/// @param cstring Source owned C string.
/// @return Borrowed slice. Empty when `cstring` is null or empty.
RuneCStringSlice rune_cstring_as_cstring_slice(RuneCString const* cstring);

/// Returns a borrowed C string slice over `ptr`.
/// @param ptr Null-terminated UTF-8 source. Null yields an empty slice.
/// @return Borrowed slice.
RuneCStringSlice rune_cstring_slice_from_ptr(char const* ptr);

/// Returns the raw borrowed pointer for `slice`.
/// @param slice Borrowed C string slice.
/// @return Null-terminated bytes, or null when empty.
char const* rune_cstring_slice_data(RuneCStringSlice slice);

/// Returns a non-owning view over the UTF-8 bytes of `slice`. Zero-copy.
/// @param out Output slice. Set to `{null, 0}` when `slice` is empty.
/// @param slice Source C string slice.
void rune_cstring_slice_as_string_slice(RuneStringSlice* out, RuneCStringSlice slice);

#ifdef __cplusplus
}
#endif
