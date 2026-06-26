#pragma once

#include <rune/forward.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Forward cursor over contiguous fixed-stride element storage.
typedef struct RuneContiguousIterator
{
    void const* data;
    usize length;
    usize element_size;
    uintptr index;
} RuneContiguousIterator;

/// Creates a cursor over `length` elements at `data`.
/// @param data Pointer to the start of the element storage.
/// @param length Number of elements in the sequence.
/// @param element_size Size in bytes between adjacent elements.
/// @return Initialised iterator positioned at the first element.
RuneContiguousIterator rune_contiguous_iterator_create(
    void const* data,
    usize length,
    usize element_size);

/// Returns whether no more elements can be read.
/// @param self Iterator to inspect.
/// @return `true` when all elements have been consumed.
bool rune_contiguous_iterator_is_empty(RuneContiguousIterator const* self);

/// Returns the number of elements remaining in the cursor.
/// @param self Iterator to inspect.
/// @return Number of elements not yet consumed.
usize rune_contiguous_iterator_remaining(RuneContiguousIterator const* self);

/// Returns the next element pointer and advances, or null when exhausted.
/// @param self Iterator to advance.
/// @return Read-only pointer to the next element, or null when exhausted.
void const* rune_contiguous_iterator_next(RuneContiguousIterator* self);

#ifdef __cplusplus
}
#endif
