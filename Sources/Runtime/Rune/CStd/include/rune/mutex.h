#pragma once

#include <rune/types.h>

#if defined(_WIN32)
#include <rune/platform/windows/mutex.h>
#else
#include <rune/platform/posix/mutex.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Initializes `self` with a newly created platform mutex.
/// @param self Mutex to create.
/// @return false when `self` is null or already initialized, true on success.
bool rune_mutex_create(RuneMutex* self);

/// Destroys the platform mutex held by `self`.
/// @param self Mutex to destroy. No-op when `self` is null or uninitialized.
void rune_mutex_destroy(RuneMutex* self);

/// Blocks the calling thread until the mutex is acquired.
/// @param self Mutex to lock.
/// @return true on success, false when `self` is null, uninitialized, or the lock fails.
bool rune_mutex_lock(RuneMutex* self);

/// Releases the mutex held by the calling thread.
/// @param self Mutex to unlock.
/// @return true on success, false when `self` is null, uninitialized, or the unlock fails.
bool rune_mutex_unlock(RuneMutex* self);

/// Returns a pointer to the embedded native mutex handle, or null when uninitialized.
/// @param self Mutex to inspect.
/// @return Pointer to the platform-native mutex object, or null when uninitialized.
void* rune_mutex_native_handle(RuneMutex* self);

#ifdef __cplusplus
}
#endif
