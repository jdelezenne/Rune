#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Function run by a `RuneThread`.
/// @param context Opaque user data passed to `rune_thread_create`.
typedef void (*RuneThreadFunction)(void* context);

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#include <rune/platform/windows/thread.h>
#else
#include <rune/platform/posix/thread.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Starts a new joinable platform thread.
/// @param self Thread storage to initialize. No-op when `self` is null or already initialized.
/// @param function Function to run on the new thread. No-op when null.
/// @param context Opaque user data passed to `function`.
/// @return True when the thread was started.
bool rune_thread_create(RuneThread* self, RuneThreadFunction function, void* context);

/// Waits for the thread to finish and releases its native handle.
/// @param self Thread to join. No-op when `self` is null or uninitialized.
void rune_thread_join(RuneThread* self);

/// Releases the native handle without waiting for the thread to finish.
/// @param self Thread to detach. No-op when `self` is null or uninitialized.
void rune_thread_detach(RuneThread* self);

/// Releases the native handle without waiting for the thread to finish.
/// @param self Thread to destroy. No-op when `self` is null or uninitialized.
void rune_thread_destroy(RuneThread* self);

/// Returns a pointer to the embedded native thread handle, or null when uninitialized.
/// @param self Thread to inspect.
/// @return Pointer to the platform-native thread object, or null when uninitialized.
void* rune_thread_native_handle(RuneThread* self);

/// Sets the current platform thread's name for debuggers and profilers.
/// @param name UTF-8 name; truncated to the platform limit when longer.
void rune_thread_set_current_name(char const* name);

#ifdef __cplusplus
}
#endif
