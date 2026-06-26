#pragma once

#include <rune/allocator.h>
#include <rune/forward.h>
#include <rune/iterator.h>
#include <rune/memory.h>
#include <rune/option.h>
#include <rune/slice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Raw allocation handle for a contiguous block of typed elements.
typedef struct RuneBuffer
{
    RuneAllocator const* allocator; ///< Allocator used for heap storage.
    usize element_size;             ///< Size of a single element in bytes.

    uint8* data;    ///< Element bytes, or null when empty.
    usize capacity; ///< Number of elements the allocated storage can hold.
} RuneBuffer;

/// Initializes an empty buffer with `element_size`.
/// @param element_size Size of a single element in bytes.
/// @return Initialised empty buffer.
RuneBuffer rune_buffer_create(usize element_size);

/// Initializes an empty buffer with `element_size` and reserved capacity for `capacity` elements.
/// @param element_size Size of a single element in bytes.
/// @param capacity Minimum element capacity.
/// @return Initialised empty buffer.
RuneBuffer rune_buffer_create_with_capacity(usize element_size, usize capacity);

/// Returns an empty buffer bound to `allocator` with `element_size`.
/// @param element_size Size of a single element in bytes.
/// @param allocator Allocator used for heap storage.
/// @return Initialised empty buffer.
RuneBuffer rune_buffer_create_with_allocator(usize element_size, RuneAllocator const* allocator);

/// Releases the backing allocation and zeroes the buffer fields.
/// @param buffer Buffer to tear down.
void rune_buffer_destroy(RuneBuffer* buffer);

/// Returns the number of elements the buffer can currently hold.
/// @param buffer Buffer to inspect.
/// @return Element capacity.
usize rune_buffer_capacity(RuneBuffer const* buffer);

/// Ensures the buffer can hold at least `element_count` elements.
/// @param buffer Buffer to grow.
/// @param element_count Minimum required element capacity.
void rune_buffer_reserve(RuneBuffer* buffer, usize element_count);

/// Returns a pointer to the element at `index`.
/// @param buffer Buffer to index into.
/// @param index Zero-based element index.
/// @return Pointer to the element bytes, or null when unavailable.
void const* rune_buffer_get(RuneBuffer const* buffer, usize index);

/// Copies `element` bytes into slot `index`. No bounds check.
/// @param buffer Target buffer.
/// @param index Zero-based element index.
/// @param element Source element bytes to copy.
void rune_buffer_set(RuneBuffer* buffer, usize index, void const* element);

/// Moves `count` elements from `src_index` to `dst_index` (memmove semantics).
/// @param buffer Buffer to operate on.
/// @param dst_index Destination zero-based element index.
/// @param src_index Source zero-based element index.
/// @param count Number of elements to move.
void rune_buffer_move_elements(
    RuneBuffer* buffer,
    usize dst_index,
    usize src_index,
    usize count);

/// Deep-copies `count` elements from `src` into `dst`.
/// @param dst Destination buffer.
/// @param src Source buffer.
/// @param count Number of elements to copy.
void rune_buffer_copy(
    RuneBuffer* dst,
    RuneBuffer const* src,
    usize count);

/// Creates a contiguous iterator over `count` elements of `buffer`.
/// @param buffer Buffer to iterate.
/// @param count Number of elements to expose.
/// @return Initialised iterator.
RuneContiguousIterator rune_buffer_iterator_create(
    RuneBuffer const* buffer,
    usize count);

/// Initialises `out` as a slice over `count` elements of `buffer`.
/// @param out Pointer to the slice structure to initialise.
/// @param buffer Source buffer.
/// @param count Number of elements to expose.
void rune_buffer_to_slice(
    RuneSlice* out,
    RuneBuffer const* buffer,
    usize count);

/// Initialises `out` as a slice over elements [start, end) of `buffer`.
/// @param out Pointer to the slice structure to initialise.
/// @param buffer Source buffer.
/// @param start Inclusive start index.
/// @param end Exclusive end index.
void rune_buffer_slice_range(
    RuneSlice* out,
    RuneBuffer const* buffer,
    usize start,
    usize end);

/// Replaces `buffer` with a copy of the bytes in `slice`.
/// @param buffer Target buffer.
/// @param slice Source slice.
void rune_buffer_from_slice(RuneBuffer* buffer, RuneSlice const* slice);

/// Returns whether any of the first `count` elements equals `element`.
/// @param buffer Buffer to search.
/// @param count Number of elements to inspect.
/// @param element Element bytes to compare.
/// @return `true` when a matching element exists.
bool rune_buffer_contains(
    RuneBuffer const* buffer,
    usize count,
    void const* element);

/// Returns the index of the first element equal to `element`.
/// @param buffer Buffer to search.
/// @param count Number of elements to inspect.
/// @param element Element bytes to compare.
/// @return Option containing the zero-based index, or empty when not found.
RuneOptionUSize rune_buffer_index_of(
    RuneBuffer const* buffer,
    usize count,
    void const* element);

#ifdef __cplusplus
}
#endif
