#pragma once

#include <rune/condition_variable.h>
#include <rune/mutex.h>
#include <rune/mutex_guard.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_runtime_condition_variable_create(RuneConditionVariable* self);
void rune_runtime_condition_variable_destroy(RuneConditionVariable* self);
void rune_runtime_condition_variable_wait(RuneConditionVariable* self, RuneMutexGuard* guard);
void rune_runtime_condition_variable_notify_one(RuneConditionVariable* self);
void rune_runtime_condition_variable_notify_all(RuneConditionVariable* self);
void* rune_runtime_condition_variable_native_handle(RuneConditionVariable* self);
void rune_runtime_mutex_guard_wait(RuneMutexGuard* guard, RuneConditionVariable* condition);

#ifdef __cplusplus
}
#endif
