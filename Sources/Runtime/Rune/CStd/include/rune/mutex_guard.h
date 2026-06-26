#pragma once

#include <rune/mutex.h>
#include <rune/types.h>

typedef struct RuneMutexGuard
{
    RuneMutex* mutex;
    bool locked;
} RuneMutexGuard;
