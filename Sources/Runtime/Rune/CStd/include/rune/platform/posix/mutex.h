#pragma once

#include <pthread.h>

/// Platform-specific storage for `RuneMutex` on POSIX (macOS, Linux).
typedef struct RuneMutex
{
    pthread_mutex_t native; ///< POSIX mutex stored inline in the struct.
    bool initialized;       ///< True after `rune_mutex_create`, false after `rune_mutex_destroy`.
} RuneMutex;
