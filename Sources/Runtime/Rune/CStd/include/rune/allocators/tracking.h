#pragma once

#include <rune/allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Allocator wrapper that records allocation and deallocation counts.
///
/// Forwards all requests to a source `RuneAllocator` and increments counters
/// on each call. Use `rune_tracking_allocator_live_count` to detect leaks, or
/// read the raw counters for allocation pressure profiling.
typedef struct RuneTrackingAllocator
{
    RuneAllocator source;  ///< Underlying allocator that fulfils requests.
    usize allocateCount;   ///< Total successful allocations since init or last reset.
    usize deallocateCount; ///< Total deallocations since init or last reset.
} RuneTrackingAllocator;

/// Creates a tracking allocator over `source` with zeroed counters.
/// @param source Underlying allocator to forward requests to.
/// @return Initialised tracking allocator.
RuneTrackingAllocator rune_tracking_allocator_create(RuneAllocator source);

/// Resets both counters to zero without touching the source allocator.
/// @param tracking Tracking allocator to reset.
void rune_tracking_allocator_reset(RuneTrackingAllocator* tracking);

/// Returns `allocateCount - deallocateCount`.
/// A non-zero value indicates live allocations that have not been freed.
/// @param tracking Tracking allocator to query.
/// @return Number of live allocations.
usize rune_tracking_allocator_live_count(RuneTrackingAllocator const* tracking);

/// Returns a `RuneAllocator` interface backed by `tracking`.
/// The returned value is only valid while `tracking` remains in scope.
/// @param tracking Tracking allocator instance.
/// @return Allocator interface for `tracking`.
RuneAllocator rune_tracking_allocator(RuneTrackingAllocator* tracking);

#ifdef __cplusplus
}
#endif
