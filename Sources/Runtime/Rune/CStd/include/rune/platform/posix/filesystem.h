#pragma once

#include <rune/path.h>
#include <rune/types.h>

/// Platform-specific directory iterator on POSIX platforms.
typedef struct RuneDirectory
{
    void*    native;      ///< `DIR*` handle.
    RunePath base;        ///< Base directory path, prepended to each entry.
    bool     initialized; ///< True after a successful `rune_directory_open`.
} RuneDirectory;
