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

/// Dynamically sized contiguous array of elements.
typedef struct RuneArray
{
    RuneAllocator const* allocator; ///< Allocator used for heap storage.
    usize element_size;             ///< Size of one element in bytes.

    uint8* data;    ///< Element bytes, or null when empty.
    usize count;    ///< Number of elements stored.
    usize capacity; ///< Number of elements the allocated storage can hold.
} RuneArray;

/// Returns an empty array.
/// @param element_size Size of a single element in bytes.
/// @return Initialised empty array.
RuneArray rune_array_create(usize element_size);

/// Returns an empty array bound to `allocator`.
/// @param element_size Size of a single element in bytes.
/// @param allocator Allocator used for heap storage.
/// @return Initialised empty array.
RuneArray rune_array_create_with_allocator(usize element_size, RuneAllocator const* allocator);

/// Creates an empty array with reserved capacity for at least `capacity` elements.
/// @param element_size Size of a single element in bytes.
/// @param capacity Minimum element capacity.
/// @return Initialised empty array.
RuneArray rune_array_create_with_capacity(usize element_size, usize capacity);

/// Creates an array with `count` copies of `element`.
/// @param element_size Element size in bytes.
/// @param element Source element bytes.
/// @param count Number of copies.
/// @return Initialised array.
RuneArray rune_array_create_repeating(
    usize element_size,
    void const* element,
    usize count);

/// Releases the backing allocation and zeroes the array fields.
/// @param array Array storage to tear down.
void rune_array_destroy(RuneArray* array);

/// Deep-copies `source` into `destination`.
/// @param destination Destination array storage.
/// @param source Source array storage.
void rune_array_copy(RuneArray* destination, RuneArray const* source);

/// Moves `source` into `destination`, leaving `source` empty.
/// @param destination Destination array storage.
/// @param source Source array storage.
void rune_array_move(RuneArray* destination, RuneArray* source);

/// Returns the number of elements in `array`.
/// @param array Array to inspect.
/// @return Element count.
usize rune_array_count(RuneArray const* array);

/// Returns a pointer to the element bytes of `array`.
/// @param array Array to inspect.
/// @return Read-only element bytes, or null when empty.
void const* rune_array_data(RuneArray const* array);

/// Returns a mutable pointer to element at `index`.
/// @param array Array to inspect.
/// @param index Zero-based index.
/// @return Element bytes, or null when out of bounds.
void* rune_array_get(RuneArray* array, usize index);

/// Replaces the element at `index` with `element` bytes.
/// @param array Target array.
/// @param index Zero-based index.
/// @param element Source element bytes.
void rune_array_set_element(RuneArray* array, usize index, void const* element);

/// Returns the used byte length of `array`.
/// @param array Array to inspect.
/// @return Byte length of stored elements.
usize rune_array_byte_size(RuneArray const* array);

/// Appends one element to `array`.
/// @param array Target array.
/// @param element Element bytes to append.
void rune_array_append(RuneArray* array, void const* element);

/// Appends all elements from `slice` to `array`.
/// @param array Target array.
/// @param slice Source slice.
void rune_array_append_slice(RuneArray* array, RuneSlice const* slice);

/// Returns whether `array` contains no elements.
/// @param array Array to inspect.
/// @return `true` when empty.
bool rune_array_is_empty(RuneArray const* array);

/// Returns whether `array` contains an element equal to `element`.
/// @param array Array to search.
/// @param element Element bytes to compare.
/// @param element_size Element size in bytes.
/// @return `true` when a matching element exists.
bool rune_array_contains_element(RuneArray const* array, void const* element);

/// Returns the zero-based index of the first element whose bytes match `element`,
/// or -1 if no match is found.
/// @param array Array to search.
/// @param element Element bytes to compare.
/// @param element_size Element size in bytes.
/// @return Option containing the zero-based index, or empty when not found.
RuneOptionUSize rune_array_index_of(RuneArray const* array, void const* element);

/// Inserts `element` at zero-based index `at`.
/// @param array Target array.
/// @param element Element bytes to insert.
/// @param element_size Element size in bytes.
/// @param at Zero-based insertion index.
void rune_array_insert_element(RuneArray* array, void const* element, usize at);

/// Removes and returns the element at zero-based index `at`.
/// @param array Target array.
/// @param at Zero-based removal index.
/// @return Option containing a pointer to removed element bytes, or empty.
RuneOptionPtr rune_array_remove(RuneArray* array, usize at);

/// Removes all elements while retaining allocated storage.
/// @param array Target array.
void rune_array_remove_all(RuneArray* array);

/// Ensures capacity for at least `minimum_capacity` elements.
/// @param array Target array.
/// @param minimum_capacity Minimum element capacity.
void rune_array_reserve(RuneArray* array, usize minimum_capacity);

/// Returns the first element pointer, or empty when the array is empty.
/// @param array Array to inspect.
/// @return Option containing a pointer to the first element, or empty.
RuneOptionPtr rune_array_first(RuneArray const* array);

/// Returns the last element pointer, or empty when the array is empty.
/// @param array Array to inspect.
/// @return Option containing a pointer to the last element, or empty.
RuneOptionPtr rune_array_last(RuneArray const* array);

/// Initialises a contiguous iterator over all elements of `array`.
/// @param out Pointer to the iterator structure to initialise.
/// @param array Array to iterate.
RuneContiguousIterator rune_array_iterator_create(RuneArray const* array);

#ifdef __cplusplus
}
#endif
