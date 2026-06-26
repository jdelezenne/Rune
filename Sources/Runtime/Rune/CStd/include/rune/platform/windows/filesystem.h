#pragma once

#include <rune/path.h>
#include <rune/platform/windows/platform.h>
#include <rune/types.h>

/// Platform-specific directory iterator on Windows.
typedef struct RuneDirectory
{
    HANDLE           handle;      ///< Handle from `FindFirstFileW`, or `INVALID_HANDLE_VALUE`.
    WIN32_FIND_DATAW find_data;   ///< Most recently read entry (set by `FindFirstFileW`/`FindNextFileW`).
    RunePath         base;        ///< Base directory path, prepended to each entry.
    bool             initialized; ///< True after a successful `rune_directory_open`.
    bool             has_pending; ///< First entry already fetched by `FindFirstFileW`.
} RuneDirectory;
