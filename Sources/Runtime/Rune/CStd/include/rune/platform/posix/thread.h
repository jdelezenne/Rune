#pragma once

#include <rune/atomic.h>

#include <pthread.h>

/// Platform-specific storage for `RuneThread` on POSIX (macOS, Linux).
typedef struct RuneThread
{
    pthread_t native;              ///< POSIX thread handle stored inline in the struct.
    RuneThreadFunction function;   ///< Entry function run by the native thread.
    void* context;                 ///< Opaque user data passed to `function`.
    RuneAtomicInt32 startupCopied; ///< Set after the native thread copies startup fields.
    bool initialized;              ///< True after `rune_thread_create`, false after join/detach/destroy.
} RuneThread;
