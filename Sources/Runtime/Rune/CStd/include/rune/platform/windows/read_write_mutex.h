#pragma once

#include <rune/platform/windows/platform.h>

/// Platform-specific storage for `RuneReadWriteMutex` on Windows.
typedef struct RuneReadWriteMutex
{
    SRWLOCK* native;   ///< Windows slim reader/writer lock handle.
    bool initialized; ///< True after `rune_read_write_mutex_create`, false after destroy.
} RuneReadWriteMutex;
