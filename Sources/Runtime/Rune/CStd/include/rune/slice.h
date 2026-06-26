#pragma once

#include <rune/forward.h>
#include <rune/iterator.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Non-owning view over a contiguous sequence of elements.
typedef struct RuneSlice
{
    void const* data;   ///< Element bytes, or null when empty.
    usize count;        ///< Number of elements in the view.
    usize element_size; ///< Size of one element in bytes.
} RuneSlice;

typedef RuneContiguousIterator RuneSliceIterator;

/// Creates an empty slice.
/// @param element_size Size of one element in bytes.
/// @return The created empty RuneSlice.
RuneSlice rune_slice_create(usize element_size);

/// Creates a non-owning slice over contiguous element storage.
/// @param element_size Size of one element in bytes.
/// @param elements Pointer to the contiguous sequence of elements.
/// @param count Number of elements.
/// @return The created RuneSlice.
RuneSlice rune_slice_create_with_elements(usize element_size, void const* elements, intptr count);

/// Creates a sub-slice covering `start..<end`.
/// @param element_size Size of one element in bytes.
/// @param slice Pointer to the source slice.
/// @param start Starting index of the sub-slice (inclusive).
/// @param end Ending index of the sub-slice (exclusive).
/// @return The created sub-slice.
RuneSlice rune_slice_create_with_range(usize element_size, RuneSlice const* slice, intptr start, intptr end);

/// Returns the number of elements in the slice.
/// @param slice Pointer to the slice.
/// @return The number of elements.
intptr rune_slice_count(RuneSlice* slice);

/// Returns whether the slice contains no elements.
/// @param slice Pointer to the slice.
/// @return True when empty, false otherwise.
bool rune_slice_is_empty(RuneSlice* slice);

/// Returns a sub-slice covering `start..<end`.
/// @param slice Pointer to the source slice.
/// @param start Starting index of the sub-slice (inclusive).
/// @param end Ending index of the sub-slice (exclusive).
/// @return The sub-slice RuneSlice.
RuneSlice rune_slice_sub(RuneSlice const* slice, intptr start, intptr end);

/// Returns a pointer to the bytes of the element at the specified index.
/// @param slice Pointer to the slice.
/// @param index Zero-based element index.
/// @return Read-only pointer to element bytes, or null when out of range.
uint8 const* rune_slice_element_bytes(RuneSlice const* slice, intptr index);

/// Returns a mutable pointer to the slice's underlying byte storage.
/// @param slice Pointer to the slice.
/// @return Mutable pointer to the underlying byte storage.
uint8* rune_slice_bytes(RuneSlice* slice);

/// Returns the total byte length of the slice storage (`count * element_size`).
/// @param slice Pointer to the slice.
/// @return The total byte length of the slice.
intptr rune_slice_byte_length(RuneSlice const* slice);

/// Returns a pointer to the element at `index`. No bounds check.
/// @param slice Pointer to the slice.
/// @param index Zero-based element index.
/// @return Pointer to the element bytes.
void* rune_slice_get(RuneSlice* slice, intptr index);

/// Replaces the element at `index` with `element`. No bounds check.
/// @param slice Pointer to the slice.
/// @param index Zero-based element index.
/// @param element Source element bytes to copy.
void rune_slice_set(RuneSlice* slice, intptr index, void const* element);

/// Returns whether the slice contains `element`.
/// @param slice Pointer to the slice.
/// @param element Element bytes to compare.
/// @return `true` when a matching element exists.
bool rune_slice_contains(RuneSlice const* slice, void const* element);

/// Creates an iterator over the slice's element storage.
/// @param slice Pointer to the source slice.
/// @return Initialised iterator.
RuneSliceIterator rune_slice_iterator_create(RuneSlice const* slice);

#ifdef __cplusplus
}
#endif
