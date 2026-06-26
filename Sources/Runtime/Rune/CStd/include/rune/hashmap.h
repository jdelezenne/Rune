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

/// Associative container mapping keys to values.
///
/// Uses open addressing with linear probing. Key and value sizes are fixed at
/// initialization and stored in `key_size` and `value_size`. `hash_fn` and
/// `equal_fn` default to byte-wise FNV hash and `memcmp` when null.
typedef struct RuneHashMap
{
    RuneAllocator const* allocator; ///< Allocator used for slot storage.
    usize key_size;                 ///< Size of one key in bytes.
    usize value_size;               ///< Size of one value in bytes.
    RuneHashFn hash_fn;             ///< Key hash function, or null for default.
    RuneEqualFn equal_fn;           ///< Key equality function, or null for default.

    uint8* slots;   ///< Slot bytes, or null when empty.
    usize count;    ///< Number of occupied entries.
    usize capacity; ///< Number of allocated slots.
} RuneHashMap;

/// Returns an empty hash map.
/// @param key_size  Size of one key in bytes.
/// @param value_size Size of one value in bytes.
/// @param hash_fn   Key hash function, or null for default byte-wise FNV hash.
/// @param equal_fn  Key equality function, or null for default byte-wise comparison.
/// @return Initialised empty hash map.
RuneHashMap rune_hashmap_create(
    usize key_size,
    usize value_size,
    RuneHashFn hash_fn,
    RuneEqualFn equal_fn);

/// Returns an empty hash map bound to `allocator`.
/// @param key_size   Size of one key in bytes.
/// @param value_size Size of one value in bytes.
/// @param hash_fn    Key hash function, or null for default byte-wise FNV hash.
/// @param equal_fn   Key equality function, or null for default byte-wise comparison.
/// @param allocator  Allocator used for slot storage.
/// @return Initialised empty hash map.
RuneHashMap rune_hashmap_create_with_allocator(
    usize key_size,
    usize value_size,
    RuneHashFn hash_fn,
    RuneEqualFn equal_fn,
    RuneAllocator const* allocator);

/// Destroys hash map storage and releases heap backing memory.
/// @param self Hash map to tear down.
void rune_hashmap_destroy(RuneHashMap* storage);

/// Deep-copies `source` into `destination`.
/// @param destination Destination hash map storage.
/// @param source Source hash map storage.
void rune_hashmap_copy(RuneHashMap* destination, RuneHashMap const* source);

/// Returns the number of key/value pairs in `self`.
/// @param self Hash map to inspect.
/// @return Entry count.
intptr rune_hashmap_count(RuneHashMap const* self);

/// Returns whether `self` contains no entries.
/// @param self Hash map to inspect.
/// @return `true` when empty.
bool rune_hashmap_is_empty(RuneHashMap const* self);

/// Ensures `self` has storage for at least `capacity` slots.
/// @param self Hash map to reserve.
/// @param capacity Minimum slot capacity.
void rune_hashmap_reserve(RuneHashMap* self, usize capacity);

/// Returns whether `self` contains `key`.
/// @param self Hash map to search.
/// @param key Key bytes to compare.
/// @return `true` when the key exists.
bool rune_hashmap_contains(RuneHashMap* self, void const* key);

/// Inserts or updates the value associated with `key`.
/// @param self Target hash map.
/// @param key Key bytes to insert.
/// @param value Value bytes to store.
void rune_hashmap_insert(RuneHashMap* self, void const* key, void const* value);

/// Removes the entry associated with `key` when present.
/// @param self Target hash map.
/// @param key Key bytes to remove.
void rune_hashmap_remove(RuneHashMap* self, void const* key);

/// Removes all entries while retaining allocated storage.
/// @param self Target hash map.
void rune_hashmap_remove_all(RuneHashMap* self);

/// Looks up `key` and returns the value pointer, or empty when absent.
/// @param self Hash map to search.
/// @param key Key bytes to look up.
/// @return Option containing a pointer to the stored value bytes, or empty.
RuneOptionPtr rune_hashmap_try_get(RuneHashMap* self, void const* key);

#ifdef __cplusplus
}
#endif
