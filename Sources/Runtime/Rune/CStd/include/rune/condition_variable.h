#pragma once

#include <rune/mutex.h>
#include <rune/types.h>

#if defined(_WIN32)
#include <rune/platform/windows/condition_variable.h>
#else
#include <rune/platform/posix/condition_variable.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Initializes `self` with a newly created platform condition variable.
/// @param self Condition variable to create. No-op when `self` is null or already initialized.
void rune_condition_variable_create(RuneConditionVariable* self);

/// Destroys the platform condition variable held by `self`.
/// @param self Condition variable to destroy. No-op when `self` is null or uninitialized.
void rune_condition_variable_destroy(RuneConditionVariable* self);

/// Waits until `self` is notified, atomically releasing and reacquiring `mutex`.
/// @param self Condition variable to wait on.
/// @param mutex Locked mutex associated with the condition predicate.
void rune_condition_variable_wait(RuneConditionVariable* self, RuneMutex* mutex);

/// Wakes one thread waiting on `self`.
/// @param self Condition variable to notify.
void rune_condition_variable_notify_one(RuneConditionVariable* self);

/// Wakes all threads waiting on `self`.
/// @param self Condition variable to notify.
void rune_condition_variable_notify_all(RuneConditionVariable* self);

/// Returns a pointer to the embedded native condition variable, or null when uninitialized.
/// @param self Condition variable to inspect.
/// @return Pointer to the platform-native condition variable object, or null when uninitialized.
void* rune_condition_variable_native_handle(RuneConditionVariable* self);

#ifdef __cplusplus
}
#endif
