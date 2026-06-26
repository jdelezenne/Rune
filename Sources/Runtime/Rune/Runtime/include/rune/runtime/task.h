#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*RuneRuntimeTaskCode)(void* context);

typedef struct RuneRuntimeTaskBody
{
    void* context;
    RuneRuntimeTaskCode code;
} RuneRuntimeTaskBody;

/// Runs `body` as a completed cooperative task on the current thread.
int64 rune_runtime_task_spawn(RuneRuntimeTaskBody body);

/// Waits for a completed cooperative task handle.
void rune_runtime_task_await(int64 handle);

/// Yields to the current cooperative scheduler.
void rune_runtime_task_yield(void);

#ifdef __cplusplus
}
#endif
