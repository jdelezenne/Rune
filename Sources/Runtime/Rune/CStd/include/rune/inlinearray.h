#pragma once

#include <rune/forward.h>
#include <rune/iterator.h>
#include <rune/slice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuneString RuneString;
typedef struct RuneCString RuneCString;
typedef RuneContiguousIterator RuneInlineArrayIterator;

/// Fixed-size array of `N` elements of type `T`.
///
/// Element count and stride are set at construction time.
/// Element storage is embedded immediately after this header in the same allocation.
/// The total size is `sizeof(RuneInlineArray) + element_count * element_size` bytes.
typedef struct RuneInlineArray
{
    uint8* data;         ///< Element storage, or null to use trailing inline storage.
    usize element_count; ///< Number of elements.
    usize element_size;  ///< Aligned size of one element in bytes.
} RuneInlineArray;

/// Records element count and stride.
/// @param element_count  Fixed element count N.
/// @param element_size Aligned stride in bytes per element.
RuneInlineArray rune_inlinearray_create(intptr element_count,
                                        usize element_size);

/// Clears the header. Embedded storage is not freed.
/// @param array Pointer to the inline array.
void rune_inlinearray_destroy(RuneInlineArray* array);

/// Copies element bytes from `src` embedded storage into `dst` embedded storage.
/// @param dst Pointer to the destination inline array.
/// @param src Pointer to the source inline array.
void rune_inlinearray_copy(RuneInlineArray* dst, RuneInlineArray const* src);

/// Returns the number of elements (N).
/// @param array Pointer to the inline array.
/// @return The number of elements.
intptr rune_inlinearray_count(RuneInlineArray const* array);

/// Returns whether the array has zero elements.
/// @param array Pointer to the inline array.
/// @return True if empty, false otherwise.
bool rune_inlinearray_is_empty(RuneInlineArray const* array);

/// Returns a pointer to element at `index`, or null if out of bounds.
/// @param array  Pointer to the inline array.
/// @param index The element index.
/// @return Pointer to the element, or null if index is out of bounds.
void* rune_inlinearray_element_ptr(RuneInlineArray* array, intptr index);

/// Returns a const pointer to element at `index`, or null if out of bounds.
/// @param array  Pointer to the inline array.
/// @param index The element index.
/// @return Const pointer to the element, or null if index is out of bounds.
void const* rune_inlinearray_const_element_ptr(RuneInlineArray const* array, intptr index);

/// Returns whether the array contains an element equal to `element`.
/// Equality is byte-wise over one element stride.
/// @param array    Pointer to the inline array.
/// @param element Pointer to the element value to search for.
/// @return True if the element is found, false otherwise.
bool rune_inlinearray_contains(RuneInlineArray const* array, void const* element);

/// Returns a `RuneSlice` view over the array elements.
/// @param array Pointer to the inline array.
/// @return A RuneSlice structure representing the entire array.
RuneSlice rune_inlinearray_to_slice(RuneInlineArray const* array);

/// Returns a `RuneSlice` view over `start..<end`.
/// @param array  Pointer to the inline array.
/// @param start The starting element index (inclusive).
/// @param end   The ending element index (exclusive).
/// @return A RuneSlice structure representing the sub-range.
RuneSlice rune_inlinearray_slice_range(RuneInlineArray const* array, intptr start, intptr end);

/// Returns a mutable pointer to the embedded element byte storage.
/// @param array Pointer to the inline array.
/// @return Pointer to the raw bytes.
uint8* rune_inlinearray_bytes(RuneInlineArray* array);

/// Returns the total byte length of embedded storage (`element_count * element_size`).
/// @param array Pointer to the inline array.
/// @return The total byte length.
intptr rune_inlinearray_byte_length(RuneInlineArray const* array);

/// Returns a pointer to the first element, or null when empty.
/// @param array Pointer to the inline array.
/// @return Pointer to the first element, or null if the array is empty.
void* rune_inlinearray_first(RuneInlineArray* array);

/// Returns a pointer to the last element, or null when empty.
/// @param array Pointer to the inline array.
/// @return Pointer to the last element, or null if the array is empty.
void* rune_inlinearray_last(RuneInlineArray* array);

/// Creates an iterator over the array's embedded element storage.
/// @param array Pointer to the inline array.
/// @return Initialised iterator.
RuneInlineArrayIterator rune_inlinearray_iterator_create(RuneInlineArray const* array);

#ifdef __cplusplus
}
#endif
