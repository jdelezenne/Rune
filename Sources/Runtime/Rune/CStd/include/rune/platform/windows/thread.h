#pragma once

#include <rune/atomic.h>
#include <rune/platform/windows/platform.h>

/// Platform-specific storage for `RuneThread` on Windows.
typedef struct RuneThread
{
    HANDLE native;                 ///< Windows thread handle.
    DWORD id;                      ///< Windows thread identifier.
    RuneThreadFunction function;   ///< Entry function run by the native thread.
    void* context;                 ///< Opaque user data passed to `function`.
    RuneAtomicInt32 startupCopied; ///< Set after the native thread copies startup fields.
    bool initialized;              ///< True after `rune_thread_create`, false after join/detach/destroy.
} RuneThread;
