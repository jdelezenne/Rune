#pragma once

#include <rune/platform/windows/platform.h>

/// Platform-specific storage for `RuneConditionVariable` on Windows.
typedef struct RuneConditionVariable
{
    CONDITION_VARIABLE native; ///< Windows condition variable stored inline in the struct.
    bool initialized;          ///< True after `rune_condition_variable_create`, false after destroy.
} RuneConditionVariable;
