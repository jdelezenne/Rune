#pragma once

#include <rune/allocator.h>
#include <rune/string.h>
#include <rune/stringslice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - RuneStringBuilder

/// Mutable, growable UTF-8 string buffer.
///
/// Accumulates text via repeated append calls and produces a `RuneString`
/// through `rune_string_builder_build`. All allocations go through the
/// stored allocator reference.
typedef struct RuneStringBuilder
{
    RuneAllocator const* allocator; ///< Allocator used for the backing buffer.
    char* data;                     ///< UTF-8 bytes, or null when empty.
    usize length;                   ///< Number of valid bytes currently in `data`.
    usize capacity;                 ///< Allocated byte capacity of `data`.
} RuneStringBuilder;

/// @name Lifetime
/// @{

/// Creates an empty builder bound to `allocator`.
/// @param allocator Allocator used for heap storage.
/// @return Initialised empty builder.
RuneStringBuilder rune_string_builder_create(RuneAllocator const* allocator);

/// Releases the backing allocation and zeroes all fields of `builder`.
/// @param builder Builder to tear down.
void rune_string_builder_destroy(RuneStringBuilder* builder);

/// @}

/// @name Capacity
/// @{

/// Ensures at least `capacity` bytes of backing storage without reallocating
/// on subsequent appends.
/// @param builder Target builder.
/// @param capacity Minimum byte capacity required.
/// @return `false` if allocation fails.
bool rune_string_builder_reserve(RuneStringBuilder* builder, usize capacity);

/// Returns the number of bytes currently accumulated in the builder.
/// @param builder Builder to inspect.
/// @return Current byte length.
usize rune_string_builder_byte_length(RuneStringBuilder const* builder);

/// @}

/// @name Appending
/// @{

/// Appends `length` raw bytes from `bytes`.
/// @param builder Target builder.
/// @param bytes Pointer to the source bytes.
/// @param length Number of bytes to append.
/// @return `false` if allocation fails.
bool rune_string_builder_append_bytes(RuneStringBuilder* builder, char const* bytes, usize length);

/// Appends the bytes of `slice`.
/// @param builder Target builder.
/// @param slice Source string slice.
/// @return `false` if allocation fails.
bool rune_string_builder_append_slice(RuneStringBuilder* builder, RuneStringSlice const* slice);

/// Appends the bytes of `string`.
/// @param builder Target builder.
/// @param string Source string.
/// @return `false` if allocation fails.
bool rune_string_builder_append_string(RuneStringBuilder* builder, RuneString const* string);

/// Encodes `codepoint` as UTF-8 and appends the result.
/// @param builder Target builder.
/// @param codepoint Unicode scalar value to encode and append.
/// @return `false` if allocation fails or the codepoint is not a valid scalar value.
bool rune_string_builder_append_codepoint(RuneStringBuilder* builder, uint32 codepoint);

/// @}

/// @name Inspection
/// @{

/// Returns a non-owning view of the current contents without finalising.
/// The slice is invalidated by any subsequent append or destroy call.
/// @param builder Builder to inspect.
/// @return Non-owning slice over the accumulated bytes.
RuneStringSlice rune_string_builder_as_slice(RuneStringBuilder const* builder);

/// @}

/// @name Finalising
/// @{

/// Transfers ownership of the accumulated bytes into `out` as a `RuneString`
/// without copying, then resets the builder to an empty state.
/// @param builder Builder to finalise.
/// @param out Receives the constructed string.
/// @return `false` when `builder` or `out` is null.
bool rune_string_builder_build(RuneStringBuilder* builder, RuneString* out);

/// Resets the accumulated length to zero without releasing the backing buffer.
/// @param builder Builder to clear.
void rune_string_builder_clear(RuneStringBuilder* builder);

/// @}

#ifdef __cplusplus
}
#endif
