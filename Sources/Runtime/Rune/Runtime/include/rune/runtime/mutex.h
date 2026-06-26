#pragma once

#include <rune/mutex.h>
#include <rune/mutex_guard.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Initializes Rune-language Mutex storage.
void rune_runtime_mutex_create(RuneMutex* self);

/// Releases Rune-language Mutex storage.
void rune_runtime_mutex_destroy(RuneMutex* self);

/// Blocks until the mutex is acquired.
void rune_runtime_mutex_lock(RuneMutex* self);

/// Releases the mutex.
void rune_runtime_mutex_unlock(RuneMutex* self);

/// Returns the platform-native mutex handle for FFI escape hatches.
void* rune_runtime_mutex_native_handle(RuneMutex* self);

/// Acquires the mutex and returns a guard for Rune scoped locking.
RuneMutexGuard rune_runtime_mutex_lock_guard(RuneMutex* self);

/// Releases a scoped mutex guard.
void rune_runtime_mutex_guard_unlock(RuneMutexGuard* self);

/// Reacquires a previously released scoped mutex guard.
void rune_runtime_mutex_guard_relock(RuneMutexGuard* self);

#ifdef __cplusplus
}
#endif
