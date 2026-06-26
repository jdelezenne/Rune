#pragma once

#include <rune/platform/windows/platform.h>
#include <rune/types.h>

#include <winsock2.h>
#include <ws2tcpip.h>

/// Platform-specific socket handle on Windows.
typedef struct RuneSocket
{
    SOCKET handle;     ///< Winsock socket handle, or `INVALID_SOCKET` when closed.
    bool   initialized; ///< True after a successful connect/listen/accept.
} RuneSocket;
