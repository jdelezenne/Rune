#pragma once

#include <rune/allocator.h>
#include <rune/forward.h>
#include <rune/option.h>
#include <rune/types.h>

RUNE_OPTION_DECLARE(RuneAllocator*, RuneOptionAllocator);

#ifdef __cplusplus
extern "C"
{
#endif

/// @name Utilities
/// @{

/// Rounds `value` up to the next multiple of `alignment`.
/// @param value Value to align.
/// @param alignment Required alignment (0 and 1 mean no padding).
/// @return Aligned value.
static inline usize rune_align_up(usize value, usize alignment)
{
    if (alignment <= 1)
    {
        return value;
    }

    return ((value + alignment - 1) / alignment) * alignment;
}

/// Returns the larger of two `usize` values.
/// @param left Left-hand value.
/// @param right Right-hand value.
/// @return `left` when `left > right`; otherwise `right`.
static inline usize rune_usize_max(usize left, usize right)
{
    return left > right ? left : right;
}

/// Returns the larger of two `uint64` values.
/// @param left Left-hand value.
/// @param right Right-hand value.
/// @return `left` when `left > right`; otherwise `right`.
static inline uint64 rune_uint64_max(uint64 left, uint64 right)
{
    return left > right ? left : right;
}

/// @}

/// @name System Allocation
/// @{

/// Allocates `size` bytes using the built-in system allocator.
/// @param size Requested size in bytes.
/// @return Allocated pointer, or null on failure.
void* rune_system_allocate(usize size);

/// Allocates `size` bytes at `alignment` using the built-in system allocator.
/// @param size Requested size in bytes.
/// @param alignment Required alignment in bytes.
/// @return Allocated pointer, or null on failure.
void* rune_system_allocate_aligned(usize size, usize alignment);

/// Deallocates memory obtained from `rune_system_allocate` or `rune_system_allocate_aligned`.
/// @param address Pointer to deallocate.
void rune_system_deallocate(void* address);

/// @}

/// @name Thread-Local Allocator Access
/// @{

/// Returns a pointer to the thread-local default allocator storage.
/// @return Pointer to default allocator storage.
RuneAllocator* rune_get_default_allocator(void);

/// Returns a pointer to the thread-local current allocator storage.
/// @return Pointer to current allocator storage.
RuneAllocator* rune_get_current_allocator(void);

/// Returns the active thread-local allocator override, or null when unset.
/// @return Current allocator override, or null.
RuneOptionAllocator rune_get_allocator(void);

/// Sets or clears the thread-local current allocator override.
/// @param allocator Pointer to `RuneAllocator`, or null to clear the override.
void rune_set_current_allocator(RuneAllocator const* allocator);

/// @}

/// @name Raw Byte Operations
/// @{

/// Zeroes `byte_count` bytes starting at `destination`.
/// @param destination Start of the region to clear.
/// @param byte_count Number of bytes to zero.
void rune_memory_zero(void* destination, usize byte_count);

/// Fills `byte_count` bytes at `destination` with `value`.
/// @param destination Start of the region to fill.
/// @param value Byte value written to each position.
/// @param byte_count Number of bytes to fill.
void rune_memory_fill(void* destination, uint8 value, usize byte_count);

/// Copies `byte_count` bytes from `source` to `destination`.
/// @param source Start of the source region.
/// @param destination Start of the destination region.
/// @param byte_count Number of bytes to copy.
void rune_memory_copy(void const* source, void* destination, usize byte_count);

/// Copies `byte_count` bytes from `source` to `destination`, allowing overlap.
/// @param source Start of the source region.
/// @param destination Start of the destination region.
/// @param byte_count Number of bytes to move.
void rune_memory_move(void const* source, void* destination, usize byte_count);

/// Lexicographically compares `byte_count` bytes.
/// @param source Start of the left-hand region.
/// @param destination Start of the right-hand region.
/// @param byte_count Number of bytes to compare.
/// @return `-1` (`Ordering.Less`), `0` (`Ordering.Equal`), or `+1` (`Ordering.Greater`).
int8_t rune_memory_compare(void const* source, void const* destination, usize byte_count);

/// @}

#ifdef __cplusplus
}
#endif
