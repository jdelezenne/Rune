#pragma once

#include <rune/allocator.h>
#include <rune/forward.h>
#include <rune/memory.h>
#include <rune/option.h>

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Collection of unique values kept in ascending order.
///
/// Backed by a red-black tree. Element size is fixed at initialization and
/// stored in `element_size`. `compare_fn` defaults to byte-wise `memcmp`
/// when null.
typedef struct RuneSortedSet
{
    RuneAllocator const* allocator; ///< Allocator used for node storage.
    void* root;                     ///< Tree root node, or null when empty.
    usize length;                   ///< Number of elements.
    usize element_size;             ///< Size of one element in bytes.
    RuneCompareFn compare_fn;       ///< Element comparator, or null for default byte-wise comparison.
} RuneSortedSet;

/// Returns an empty sorted set bound to `allocator`.
/// @param element_size Size of one element in bytes.
/// @param compare_fn   Element comparator, or null for default byte-wise comparison.
/// @param allocator    Allocator used for node storage.
/// @return Initialised empty sorted set.
RuneSortedSet rune_sortedset_create(
    usize element_size,
    RuneCompareFn compare_fn,
    RuneAllocator const* allocator);

/// Destroys sorted set storage and releases heap backing memory.
/// @param self Sorted set to tear down.
void rune_sortedset_destroy(RuneSortedSet* storage);

/// Deep-copies `source` into `destination`.
/// @param destination Destination sorted set storage.
/// @param source Source sorted set storage.
void rune_sortedset_copy(RuneSortedSet* destination, RuneSortedSet const* source);

/// Returns the number of elements in `self`.
/// @param self Sorted set to inspect.
/// @return Element count.
intptr rune_sortedset_count(RuneSortedSet const* self);

/// Returns whether `self` contains no elements.
/// @param self Sorted set to inspect.
/// @return `true` when empty.
bool rune_sortedset_is_empty(RuneSortedSet const* self);

/// Inserts `element` when not already present, or updates the stored value.
/// @param self Target sorted set.
/// @param element Element bytes to insert.
void rune_sortedset_insert(RuneSortedSet* self, void const* element);

/// Removes `element` when present.
/// @param self Target sorted set.
/// @param element Element bytes to remove.
void rune_sortedset_remove(RuneSortedSet* self, void const* element);

/// Removes all elements from `self`.
/// @param self Target sorted set.
void rune_sortedset_remove_all(RuneSortedSet* self);

/// Returns whether `self` contains `element`.
/// @param self Sorted set to search.
/// @param element Element bytes to compare.
/// @return `true` when the element exists.
bool rune_sortedset_contains(RuneSortedSet* self, void const* element);

/// Returns the smallest element bytes.
/// @param self Sorted set to inspect.
/// @return Option containing read-only element bytes, or empty.
RuneOptionPtr rune_sortedset_first(RuneSortedSet const* self);

/// Returns the next larger element after `current`.
/// @param self Sorted set to inspect.
/// @param current Current element bytes returned by `rune_sortedset_first` or `rune_sortedset_next`.
/// @return Option containing read-only element bytes, or empty when `current` is the last element.
RuneOptionPtr rune_sortedset_next(RuneSortedSet const* self, uint8 const* current);

/// In-order cursor over the elements of a `RuneSortedSet`.
typedef struct RuneSortedSetIterator
{
    RuneSortedSet const* set; ///< Set being iterated.
    uint8 const* current;     ///< Element bytes most recently yielded, or null before the first call.
    bool started;             ///< Whether `rune_sortedset_iterator_next` has been called yet.
} RuneSortedSetIterator;

/// Creates a cursor positioned before the smallest element of `set`.
/// @param out Uninitialised iterator storage.
/// @param set Sorted set to iterate.
void rune_sortedset_iterator_create(RuneSortedSetIterator* out, RuneSortedSet const* set);

/// Advances to the next element in ascending order and returns a pointer to
/// its bytes, or null when exhausted.
/// @param self Iterator to advance.
/// @return Read-only pointer to the element bytes, or null when exhausted.
void const* rune_sortedset_iterator_next(RuneSortedSetIterator* self);

#ifdef __cplusplus
}
#endif
