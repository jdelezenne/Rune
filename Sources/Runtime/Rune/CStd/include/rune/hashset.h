#pragma once

#include <rune/allocator.h>
#include <rune/forward.h>
#include <rune/memory.h>

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Collection of unique values.
///
/// Uses open addressing with linear probing. Element size is fixed at
/// initialization and stored in `element_size`. `hash_fn` and `equal_fn`
/// default to byte-wise FNV hash and `memcmp` when null.
typedef struct RuneHashSet
{
    RuneAllocator const* allocator; ///< Allocator used for slot storage.
    usize element_size;             ///< Size of one element in bytes.
    RuneHashFn hash_fn;             ///< Element hash function, or null for default.
    RuneEqualFn equal_fn;           ///< Element equality function, or null for default.

    uint8* slots;   ///< Slot bytes, or null when empty.
    usize count;    ///< Number of occupied entries.
    usize capacity; ///< Number of allocated slots.
} RuneHashSet;

/// Returns an empty hash set.
/// @param element_size Size of one element in bytes.
/// @param hash_fn      Element hash function, or null for default byte-wise FNV hash.
/// @param equal_fn     Element equality function, or null for default byte-wise comparison.
/// @return Initialised empty hash set.
RuneHashSet rune_hashset_create(usize element_size, RuneHashFn hash_fn, RuneEqualFn equal_fn);

/// Returns an empty hash set bound to `allocator`.
/// @param element_size Size of one element in bytes.
/// @param hash_fn      Element hash function, or null for default byte-wise FNV hash.
/// @param equal_fn     Element equality function, or null for default byte-wise comparison.
/// @param allocator    Allocator used for slot storage.
/// @return Initialised empty hash set.
RuneHashSet rune_hashset_create_with_allocator(
    usize element_size,
    RuneHashFn hash_fn,
    RuneEqualFn equal_fn,
    RuneAllocator const* allocator);

/// Destroys hash set storage and releases heap backing memory.
/// @param self Hash set to tear down.
void rune_hashset_destroy(RuneHashSet* storage);

/// Deep-copies `source` into `destination`.
/// @param destination Destination hash set storage.
/// @param source Source hash set storage.
void rune_hashset_copy(RuneHashSet* destination, RuneHashSet const* source);

/// Returns the number of elements in `self`.
/// @param self Hash set to inspect.
/// @return Element count.
intptr rune_hashset_count(RuneHashSet const* self);

/// Returns whether `self` contains no elements.
/// @param self Hash set to inspect.
/// @return `true` when empty.
bool rune_hashset_is_empty(RuneHashSet const* self);

/// Ensures `self` has storage for at least `capacity` slots.
/// @param self Hash set to reserve.
/// @param capacity Minimum slot capacity.
void rune_hashset_reserve(RuneHashSet* self, usize capacity);

/// Inserts `element` when not already present.
/// @param self Target hash set.
/// @param element Element bytes to insert.
void rune_hashset_insert(RuneHashSet* self, void const* element);

/// Removes `element` when present.
/// @param self Target hash set.
/// @param element Element bytes to remove.
void rune_hashset_remove(RuneHashSet* self, void const* element);

/// Removes all elements while retaining allocated storage.
/// @param self Target hash set.
void rune_hashset_remove_all(RuneHashSet* self);

/// Returns whether `self` contains `element`.
/// @param self Hash set to search.
/// @param element Element bytes to compare.
/// @return `true` when the element exists.
bool rune_hashset_contains(RuneHashSet* self, void const* element);

#ifdef __cplusplus
}
#endif
