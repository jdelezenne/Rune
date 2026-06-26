#pragma once

#include <rune/allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Bump-pointer allocator backed by virtual memory with on-demand page commitment.
///
/// Reserves a large virtual address range upfront without committing physical
/// pages. Pages are committed as the allocation cursor advances, so only memory
/// actually touched consumes physical RAM.
///
/// Unlike `RuneArenaAllocator`, which requires a fully committed caller-provided
/// buffer, this allocator owns its address range and must be disposed:
/// - `reset` decommits all pages, returning physical memory to the OS while
///   keeping the virtual reservation intact for reuse.
/// - `dispose` must be called to release the virtual address range.
typedef struct RuneVirtualAllocator
{
    byte* base;      ///< Reserved virtual base address.
    usize reserved;  ///< Total virtual address range in bytes.
    usize committed; ///< Bytes of physical pages committed so far.
    usize offset;    ///< Allocation cursor in bytes.
    usize pageSize;  ///< OS page size, cached at make time.
} RuneVirtualAllocator;

/// Reserves `reserveSize` bytes of virtual address space.
/// No physical memory is committed until the first allocation.
/// @param allocator  Virtual allocator to initialise.
/// @param reserveSize Bytes of virtual address space to reserve.
/// @return `true` on success, `false` if the OS reservation failed.
bool rune_virtual_allocator_create(RuneVirtualAllocator* allocator, usize reserveSize);

/// Decommits all committed pages, returning physical memory to the OS.
/// The virtual reservation is preserved; allocations can resume immediately.
/// @param allocator Virtual allocator to reset.
void rune_virtual_allocator_reset(RuneVirtualAllocator* allocator);

/// Releases the virtual address reservation.
/// Must be called when the allocator is no longer needed.
/// @param allocator Virtual allocator to dispose.
void rune_virtual_allocator_destroy(RuneVirtualAllocator* allocator);

/// Returns the number of bytes still allocatable within the virtual reservation.
/// @param allocator Virtual allocator to query.
/// @return Remaining bytes.
usize rune_virtual_allocator_remaining(RuneVirtualAllocator const* allocator);

/// Returns a `RuneAllocator` interface backed by `allocator`.
/// The returned value is only valid while `allocator` remains in scope.
/// @param allocator Virtual allocator instance.
/// @return Allocator interface.
RuneAllocator rune_virtual_allocator(RuneVirtualAllocator* allocator);

#ifdef __cplusplus
}
#endif
