#pragma once

#include <pthread.h>

/// Platform-specific storage for `RuneConditionVariable` on POSIX (macOS, Linux).
typedef struct RuneConditionVariable
{
    pthread_cond_t native; ///< POSIX condition variable stored inline in the struct.
    bool initialized;     ///< True after `rune_condition_variable_create`, false after destroy.
} RuneConditionVariable;
