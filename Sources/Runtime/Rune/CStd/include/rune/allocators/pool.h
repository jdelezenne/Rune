#pragma once

#include <rune/allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Fixed-size block allocator backed by a caller-provided buffer.
///
/// Maintains an intrusive free-list of equal-sized blocks; allocation and
/// deallocation are both O(1) with no per-block metadata overhead.
/// `blockSize` must be at least `sizeof(void*)`. The buffer must be at least
/// `blockCount * blockSize` bytes.
/// Ideal for game objects, components, or any homogeneous high-churn type.
typedef struct RunePoolAllocator
{
    void* freeList;   ///< Head of the intrusive free-list (null when empty).
    usize blockSize;  ///< Size in bytes of each block.
    usize blockCount; ///< Total number of blocks in the pool.
    usize freeCount;  ///< Number of blocks currently available.
} RunePoolAllocator;

/// Creates a pool managing `blockCount` fixed-size blocks of `blockSize` bytes.
/// @param buffer Caller-provided backing buffer; must be at least `blockCount * blockSize` bytes.
/// @param blockSize Size in bytes of each block; must be at least `sizeof(void*)`.
/// @param blockCount Number of blocks in the pool.
/// @return Initialised pool allocator.
RunePoolAllocator rune_pool_allocator_create(void* buffer, usize blockSize, usize blockCount);

/// Returns the number of blocks currently available.
/// @param pool Pool to query.
/// @return Number of free blocks.
usize rune_pool_allocator_free_count(RunePoolAllocator const* pool);

/// Returns a `RuneAllocator` interface backed by `pool`.
/// The returned value is only valid while `pool` remains in scope.
/// Requests larger than `blockSize` return null.
/// @param pool Pool allocator instance.
/// @return Allocator interface for `pool`.
RuneAllocator rune_pool_allocator(RunePoolAllocator* pool);

#ifdef __cplusplus
}
#endif
