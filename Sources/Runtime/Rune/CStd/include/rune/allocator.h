#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Opaque type carrying per-instance state for a `RuneAllocator`.
///
/// Each allocator implementation defines its own concrete struct and casts
/// to/from `RuneAllocatorContext*` inside its `allocate` and `deallocate`
/// callbacks. Callers never dereference this pointer directly.
typedef struct RuneAllocatorContext RuneAllocatorContext;

/// Interface for memory allocation and deallocation.
///
/// A `RuneAllocator` is a pair of function pointers plus an opaque `context`
/// pointer that acts as the allocator instance ("self"). Heap-backed standard
/// library types store a `RuneAllocator` and route all backing storage through
/// it, enabling custom allocators (arenas, pools, tracking wrappers, …) to be
/// injected at construction time or via the thread-local override.
///
/// ## Implementing a custom allocator
///
/// Define a concrete context struct, write `allocate` / `deallocate` functions
/// whose first parameter is `RuneAllocatorContext*`, cast to the concrete type
/// inside, and fill a `RuneAllocator` with a pointer to the context and the two
/// function pointers.
typedef struct RuneAllocator
{
    /// Per-instance state passed as the first argument to every callback.
    RuneAllocatorContext* context;

    /// Allocates memory of the specified size and alignment.
    /// @param context Pointer to the allocator's context.
    /// @param size Requested size in bytes.
    /// @param alignment Required alignment in bytes.
    /// @return Allocated pointer, or null on failure.
    void* (*allocate)(RuneAllocatorContext* context, usize size, usize alignment);

    /// Reallocates a previously allocated block of memory to a new size.
    /// @param context Pointer to the allocator's context.
    /// @param pointer Existing pointer, or null to allocate.
    /// @param oldSize Existing allocation size in bytes.
    /// @param newSize Requested size in bytes.
    /// @param alignment Required alignment in bytes.
    /// @return Reallocated pointer, or null on failure.
    void* (*reallocate)(
        RuneAllocatorContext* context,
        void* pointer,
        usize oldSize,
        usize newSize,
        usize alignment);

    /// Deallocates a previously allocated block of memory.
    /// @param context Pointer to the allocator's context.
    /// @param pointer Pointer to the memory block to free.
    void (*deallocate)(RuneAllocatorContext* context, void* pointer);

    /// Allocates aligned storage of the specified size and alignment.
    /// @param context Pointer to the allocator's context.
    /// @param size Requested size in bytes.
    /// @param alignment Required alignment in bytes.
    /// @return Allocated pointer, or null on failure.
    void* (*allocateAligned)(RuneAllocatorContext* context, usize size, usize alignment);

    /// Reallocates a previously aligned block of memory to a new size.
    /// @param context Pointer to the allocator's context.
    /// @param pointer Existing pointer, or null to allocate.
    /// @param oldSize Existing allocation size in bytes.
    /// @param newSize Requested size in bytes.
    /// @param alignment Required alignment in bytes.
    /// @return Reallocated pointer, or null on failure.
    void* (*reallocateAligned)(
        RuneAllocatorContext* context,
        void* pointer,
        usize oldSize,
        usize newSize,
        usize alignment);

    /// Deallocates a previously aligned block of memory.
    /// @param context Pointer to the allocator's context.
    /// @param pointer Pointer to the memory block to free.
    /// @param alignment Required alignment in bytes.
    void (*deallocateAligned)(RuneAllocatorContext* context, void* pointer, usize alignment);
} RuneAllocator;

/// Returns whether `allocator` provides both allocate and deallocate operations.
/// @param allocator Allocator to inspect.
/// @return `true` when both function pointers are set.
bool rune_allocator_is_valid(RuneAllocator const* allocator);

/// Allocates `size` bytes with `alignment` through `allocator`.
/// @param allocator Active allocator.
/// @param size Requested size in bytes.
/// @param alignment Required alignment in bytes.
/// @return Allocated pointer, or null on failure or invalid allocator.
void* rune_allocator_allocate(RuneAllocator const* allocator, usize size, usize alignment);

/// Reallocates `pointer` through `allocator`.
/// @param allocator Active allocator.
/// @param pointer Existing pointer, or null to allocate.
/// @param oldSize Existing allocation size in bytes.
/// @param newSize Requested size in bytes.
/// @param alignment Required alignment in bytes.
/// @return Reallocated pointer, or null on failure.
void* rune_allocator_reallocate(
    RuneAllocator const* allocator,
    void* pointer,
    usize oldSize,
    usize newSize,
    usize alignment);

/// Deallocates `pointer` through `allocator`.
/// @param allocator Active allocator.
/// @param pointer Pointer previously returned by `rune_allocator_allocate`.
void rune_allocator_deallocate(RuneAllocator const* allocator, void* pointer);

/// Allocates aligned storage through `allocator`.
/// @param allocator Active allocator.
/// @param size Requested size in bytes.
/// @param alignment Required alignment in bytes.
/// @return Allocated pointer, or null on failure or invalid allocator.
void* rune_allocator_allocate_aligned(RuneAllocator const* allocator, usize size, usize alignment);

/// Reallocates aligned storage through `allocator`.
/// @param allocator Active allocator.
/// @param pointer Existing pointer, or null to allocate.
/// @param oldSize Existing allocation size in bytes.
/// @param newSize Requested size in bytes.
/// @param alignment Required alignment in bytes.
/// @return Reallocated pointer, or null on failure.
void* rune_allocator_reallocate_aligned(
    RuneAllocator const* allocator,
    void* pointer,
    usize oldSize,
    usize newSize,
    usize alignment);

/// Deallocates aligned storage through `allocator`.
/// @param allocator Active allocator.
/// @param pointer Pointer previously returned by `rune_allocator_allocate_aligned`.
/// @param alignment Required alignment in bytes.
void rune_allocator_deallocate_aligned(RuneAllocator const* allocator, void* pointer, usize alignment);

/// Returns the thread-local default allocator.
/// @return Default allocator snapshot.
RuneAllocator rune_allocator_default(void);

/// Returns the thread-local current allocator override, falling back to the default.
/// @return Current allocator snapshot.
RuneAllocator rune_allocator_current(void);

/// Sets the thread-local current allocator override.
/// Pass null to clear the override and restore the default.
/// @param allocator Allocator to install as the current override, or null to clear.
void rune_allocator_set_current(RuneAllocator const* allocator);

#ifdef __cplusplus
}
#endif
