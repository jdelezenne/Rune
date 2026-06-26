#pragma once

#include <rune/types.h>

/// Platform-specific file handle on Darwin.
typedef struct RuneFile
{
    int  fd;          ///< POSIX file descriptor, or -1 when closed.
    bool initialized; ///< True after a successful `rune_file_open`.
} RuneFile;
