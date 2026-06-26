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

/// Associative container mapping keys to values in ascending key order.
///
/// Backed by a red-black tree. Key and value sizes are fixed at initialization
/// and stored in `key_size` and `value_size`. `compare_fn` defaults to
/// byte-wise `memcmp` when null.
typedef struct RuneSortedMap
{
    RuneAllocator const* allocator; ///< Allocator used for node storage.
    void* root;                     ///< Tree root node, or null when empty.
    usize length;                   ///< Number of occupied entries.
    usize key_size;                 ///< Size of one key in bytes.
    usize value_size;               ///< Size of one value in bytes.
    RuneCompareFn compare_fn;       ///< Key comparator, or null for default byte-wise comparison.
} RuneSortedMap;

/// Returns an empty sorted map bound to `allocator`.
/// @param key_size   Size of one key in bytes.
/// @param value_size Size of one value in bytes.
/// @param compare_fn Key comparator, or null for default byte-wise comparison.
/// @param allocator  Allocator used for node storage.
/// @return Initialised empty sorted map.
RuneSortedMap rune_sortedmap_create(
    usize key_size,
    usize value_size,
    RuneCompareFn compare_fn,
    RuneAllocator const* allocator);

/// Destroys sorted map storage and releases heap backing memory.
/// @param self Sorted map to tear down.
void rune_sortedmap_destroy(RuneSortedMap* storage);

/// Deep-copies `source` into `destination`.
/// @param destination Destination sorted map storage.
/// @param source Source sorted map storage.
void rune_sortedmap_copy(RuneSortedMap* destination, RuneSortedMap const* source);

/// Returns the number of key/value pairs in `self`.
/// @param self Sorted map to inspect.
/// @return Entry count.
intptr rune_sortedmap_count(RuneSortedMap const* self);

/// Returns whether `self` contains no entries.
/// @param self Sorted map to inspect.
/// @return `true` when empty.
bool rune_sortedmap_is_empty(RuneSortedMap const* self);

/// Returns whether `self` contains `key`.
/// @param self Sorted map to search.
/// @param key Key bytes to compare.
/// @return `true` when the key exists.
bool rune_sortedmap_contains(RuneSortedMap* self, void const* key);

/// Inserts or updates the value associated with `key`.
/// @param self Target sorted map.
/// @param key Key bytes to insert.
/// @param value Value bytes to store.
void rune_sortedmap_insert(RuneSortedMap* self, void const* key, void const* value);

/// Removes the entry associated with `key` when present.
/// @param self Target sorted map.
/// @param key Key bytes to remove.
void rune_sortedmap_remove(RuneSortedMap* self, void const* key);

/// Looks up `key` and returns the value pointer, or empty when absent.
/// @param self Sorted map to search.
/// @param key Key bytes to look up.
/// @return Option containing a pointer to the stored value bytes, or empty.
RuneOptionPtr rune_sortedmap_try_get(RuneSortedMap* self, void const* key);

/// Returns the smallest key bytes.
/// @param self Sorted map to inspect.
/// @return Option containing read-only key bytes, or empty.
RuneOptionPtr rune_sortedmap_first_key(RuneSortedMap const* self);

/// Returns the next larger key after `current`.
/// @param self Sorted map to inspect.
/// @param current Current key bytes returned by `rune_sortedmap_first_key` or `rune_sortedmap_next_key`.
/// @return Option containing read-only key bytes, or empty when `current` is the last key.
RuneOptionPtr rune_sortedmap_next_key(RuneSortedMap const* self, uint8 const* current);

#ifdef __cplusplus
}
#endif
