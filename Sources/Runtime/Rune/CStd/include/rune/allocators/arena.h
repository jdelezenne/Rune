#pragma once

#include <rune/allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Linear bump-pointer allocator backed by a caller-provided buffer.
///
/// Advances a cursor through the buffer on each allocation; deallocation is a
/// no-op. All memory is reclaimed at once with `rune_arena_allocator_reset`.
/// Ideal for per-frame scratch memory or any allocation pattern where lifetime
/// is scoped rather than tied to individual objects.
typedef struct RuneArenaAllocator
{
    byte* buffer;   ///< Start of the backing buffer.
    usize capacity; ///< Total size of the backing buffer in bytes.
    usize offset;   ///< Current allocation cursor in bytes.
} RuneArenaAllocator;

/// Creates an arena over `buffer`.
/// @param buffer Caller-provided backing buffer.
/// @param capacity Total size of `buffer` in bytes.
/// @return Initialised arena allocator.
RuneArenaAllocator rune_arena_allocator_create(void* buffer, usize capacity);

/// Resets the arena cursor, making the full buffer available again.
/// Does not clear the buffer contents.
/// @param arena Arena allocator to reset.
void rune_arena_allocator_reset(RuneArenaAllocator* arena);

/// Returns the number of bytes still available in `arena`.
/// @param arena Arena to query.
/// @return Remaining bytes.
usize rune_arena_allocator_remaining(RuneArenaAllocator const* arena);

/// Returns a `RuneAllocator` interface backed by `arena`.
/// The returned value is only valid while `arena` remains in scope.
/// @param arena Arena allocator instance.
/// @return Allocator interface for `arena`.
RuneAllocator rune_arena_allocator(RuneArenaAllocator* arena);

#ifdef __cplusplus
}
#endif
