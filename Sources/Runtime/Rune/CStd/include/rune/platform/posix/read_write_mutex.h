#pragma once

#include <pthread.h>

/// Platform-specific storage for `RuneReadWriteMutex` on POSIX (macOS, Linux).
typedef struct RuneReadWriteMutex
{
    pthread_rwlock_t* native; ///< POSIX read-write lock handle.
    bool initialized;         ///< True after `rune_read_write_mutex_create`, false after destroy.
} RuneReadWriteMutex;
