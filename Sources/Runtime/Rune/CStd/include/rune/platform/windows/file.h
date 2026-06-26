#pragma once

#include <rune/platform/windows/platform.h>
#include <rune/types.h>

/// Platform-specific file handle on Windows.
typedef struct RuneFile
{
    HANDLE handle;    ///< Windows file handle, or `INVALID_HANDLE_VALUE` when closed.
    bool   initialized; ///< True after a successful `rune_file_open`.
} RuneFile;
