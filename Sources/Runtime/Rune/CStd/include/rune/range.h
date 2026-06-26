#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Range variants used when normalizing slice bounds.
typedef enum RuneRangeVariant
{
    RuneRangeVariant_Range = 0,            ///< Half-open interval from start to end.
    RuneRangeVariant_RangeInclusive = 1,   ///< Closed interval from start through end.
    RuneRangeVariant_RangeTo = 2,          ///< Half-open interval from the beginning through end.
    RuneRangeVariant_RangeFrom = 3,        ///< Half-open interval from start through the end.
    RuneRangeVariant_RangeToInclusive = 4, ///< Closed interval from the start through end.
} RuneRangeVariant;

/// Range used to describe a slice over a collection.
typedef struct RuneRange
{
    int64 start;              ///< Range start, or ignored for prefix ranges.
    int64 end;                ///< Range end, or ignored for suffix ranges.
    RuneRangeVariant variant; ///< One of `RuneRangeVariant`.
} RuneRange;

/// Iterator over integer values within a range.
typedef struct RuneRangeIterator
{
    int64 current;            ///< The current value of the iterator.
    int64 end;                ///< The end value (inclusive or exclusive depending on the variant).
    RuneRangeVariant variant; ///< The variant of the range being iterated.
    bool exhausted;           ///< Whether the iterator has finished yielding elements.
} RuneRangeIterator;

/// Half-open index bounds for a slice.
typedef struct RuneSliceBounds
{
    int64 start;         ///< Inclusive start index.
    int64 end_exclusive; ///< Exclusive end index.
} RuneSliceBounds;

/// Creates a range value from raw bounds and a variant.
/// @param start Range start, or ignored for prefix ranges.
/// @param end Range end, or ignored for suffix ranges.
/// @param variant One of `RuneRangeVariant`.
/// @return A new RuneRange struct initialized with the specified parameters.
RuneRange rune_range_create(int64 start, int64 end, RuneRangeVariant variant);

/// Converts `range` into clamped half-open bounds for a collection of `length`.
/// @param range Range to normalize.
/// @param length Collection length used for clamping and suffix ranges.
/// @return Normalized slice bounds.
RuneSliceBounds rune_range_normalize(RuneRange range, int64 length);

/// Returns the number of integer elements in the range.
/// @param range A pointer to the range to count.
/// @return The count of elements, or 0 if the range pointer is null or infinite.
int64 rune_range_count(RuneRange const* range);

/// Returns true when the range contains no integer elements.
/// @param range A pointer to the range to check.
/// @return True if the range has zero elements, false otherwise.
bool rune_range_is_empty(RuneRange const* range);

/// Returns the number of integer elements in the inclusive range.
/// @param range Pointer to the range to count.
/// @return Element count, or 0 when range is null or empty.
int64 rune_range_inclusive_count(RuneRange const* range);

/// Returns true when the inclusive range contains no integer elements.
/// @param range Pointer to the range to check.
/// @return True if the range has zero elements, false otherwise.
bool rune_range_inclusive_is_empty(RuneRange const* range);

/// Returns the integer element at zero-based offset index.
/// @param range A pointer to the range.
/// @param index The zero-based index of the element to retrieve.
/// @return The element value at the given index, or 0 if index is out of bounds or range is null.
int64 rune_range_get(RuneRange const* range, int64 index);

/// Returns the integer element at zero-based offset `index` in the inclusive range.
/// @param range Pointer to the range.
/// @param index Zero-based index of the element to retrieve.
/// @return Element value at the given index, or 0 when out of bounds or range is null.
int64 rune_range_inclusive_get(RuneRange const* range, int64 index);

/// Creates an iterator over any finite range variant.
/// @param range A pointer to the range to iterate over.
/// @return Initialised iterator.
RuneRangeIterator rune_range_iterator_create(RuneRange const* range);

/// Alias used by RangeInclusive.makeIterator.
/// @param range A pointer to the range to iterate over.
/// @return Initialised iterator.
RuneRangeIterator rune_range_inclusive_iterator_create(RuneRange const* range);

/// Creates an iterator over a prefix range.
/// @param range A pointer to the range to iterate over.
/// @return Initialised iterator.
RuneRangeIterator rune_range_to_iterator_create(RuneRange const* range);

/// Creates an iterator over a suffix range.
/// @param range A pointer to the range to iterate over.
/// @return Initialised iterator.
RuneRangeIterator rune_range_from_iterator_create(RuneRange const* range);

/// Creates an iterator over an inclusive prefix range.
/// @param range A pointer to the range to iterate over.
/// @return Initialised iterator.
RuneRangeIterator rune_range_to_inclusive_iterator_create(RuneRange const* range);

/// Creates a range iterator from explicit bounds.
/// @param start The starting value of the range.
/// @param end The ending value of the range.
/// @param inclusive True if the end value should be included in the iteration.
/// @return Initialised iterator.
RuneRangeIterator rune_range_iterator_create_with_bounds(int64 start, int64 end, bool inclusive);

/// Advances the iterator. Returns true and writes out_value when an element exists.
/// @param self Pointer to the iterator.
/// @param out_value Pointer to where the next value will be written.
/// @return True if another element was retrieved; false if the iterator is exhausted.
bool rune_range_iterator_next(RuneRangeIterator* self, int64* out_value);

#ifdef __cplusplus
}
#endif
