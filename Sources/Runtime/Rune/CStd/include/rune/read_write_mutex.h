#pragma once

#include <rune/types.h>

#if defined(_WIN32)
#include <rune/platform/windows/read_write_mutex.h>
#else
#include <rune/platform/posix/read_write_mutex.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Initializes `self` with a newly created platform read-write mutex.
/// @param self Read-write mutex to create.
/// @return false when `self` is null or already initialized, true on success.
bool rune_read_write_mutex_create(RuneReadWriteMutex* self);

/// Destroys the platform read-write mutex held by `self`.
/// @param self Read-write mutex to destroy. No-op when `self` is null or uninitialized.
void rune_read_write_mutex_destroy(RuneReadWriteMutex* self);

/// Blocks the calling thread until a shared/read lock is acquired.
/// @param self Read-write mutex to lock for reading.
/// @return true on success, false when `self` is null, uninitialized, or the lock fails.
bool rune_read_write_mutex_lock_read(RuneReadWriteMutex* self);

/// Releases a shared/read lock held by the calling thread.
/// @param self Read-write mutex to unlock.
/// @return true on success, false when `self` is null, uninitialized, or the unlock fails.
bool rune_read_write_mutex_unlock_read(RuneReadWriteMutex* self);

/// Blocks the calling thread until an exclusive/write lock is acquired.
/// @param self Read-write mutex to lock for writing.
/// @return true on success, false when `self` is null, uninitialized, or the lock fails.
bool rune_read_write_mutex_lock_write(RuneReadWriteMutex* self);

/// Releases an exclusive/write lock held by the calling thread.
/// @param self Read-write mutex to unlock.
/// @return true on success, false when `self` is null, uninitialized, or the unlock fails.
bool rune_read_write_mutex_unlock_write(RuneReadWriteMutex* self);

/// Returns a pointer to the native read-write mutex handle, or null when uninitialized.
/// @param self Read-write mutex to inspect.
/// @return Pointer to the platform-native object, or null when uninitialized.
void* rune_read_write_mutex_native_handle(RuneReadWriteMutex* self);

#ifdef __cplusplus
}
#endif
