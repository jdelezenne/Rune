#pragma once

#include <rune/types.h>

/// Platform-specific socket handle on POSIX platforms.
typedef struct RuneSocket
{
    int  fd;          ///< POSIX socket descriptor, or -1 when closed.
    bool initialized; ///< True after a successful connect/listen/accept.
} RuneSocket;
