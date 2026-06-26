#pragma once

#include <rune/read_write_mutex.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Initializes Rune-language ReadWriteMutex storage.
void rune_runtime_read_write_mutex_create(RuneReadWriteMutex* self);

/// Releases Rune-language ReadWriteMutex storage.
void rune_runtime_read_write_mutex_destroy(RuneReadWriteMutex* self);

/// Blocks until a shared/read lock is acquired.
void rune_runtime_read_write_mutex_lock_read(RuneReadWriteMutex* self);

/// Releases a shared/read lock.
void rune_runtime_read_write_mutex_unlock_read(RuneReadWriteMutex* self);

/// Blocks until an exclusive/write lock is acquired.
void rune_runtime_read_write_mutex_lock_write(RuneReadWriteMutex* self);

/// Releases an exclusive/write lock.
void rune_runtime_read_write_mutex_unlock_write(RuneReadWriteMutex* self);

/// Returns the platform-native read-write mutex handle for FFI escape hatches.
void* rune_runtime_read_write_mutex_native_handle(RuneReadWriteMutex* self);

#ifdef __cplusplus
}
#endif
