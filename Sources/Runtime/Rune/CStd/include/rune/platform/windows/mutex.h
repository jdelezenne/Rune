#pragma once

#include <rune/platform/windows/platform.h>

/// Platform-specific storage for `RuneMutex` on Windows.
typedef struct RuneMutex
{
    CRITICAL_SECTION native; ///< Windows critical section stored inline in the struct.
    bool initialized;        ///< True after `rune_mutex_create`, false after `rune_mutex_destroy`.
} RuneMutex;
