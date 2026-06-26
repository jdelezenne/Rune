#pragma once

#include <rune/string.h>
#include <rune/thread.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Backend-emitted adapter for a Rune `() -> Void` function value.
typedef void (*RuneRuntimeThreadCode)(void* context);

/// Runtime ABI shape of a Rune closure/function value.
typedef struct RuneRuntimeThreadBody
{
    void* context;
    RuneRuntimeThreadCode code;
} RuneRuntimeThreadBody;

/// Starts a joinable OS thread running `body`.
void rune_runtime_thread_spawn(RuneThread* self, RuneRuntimeThreadBody body);

/// Starts a joinable OS thread after copying `body.context` into owned storage.
/// Used by backends for escaping captured closures.
void rune_runtime_thread_spawn_with_context(
    RuneThread* self,
    RuneRuntimeThreadBody body,
    usize contextSize,
    usize contextAlignment);

/// Starts a joinable OS thread running `body` after setting the thread name.
void rune_runtime_thread_spawn_named(RuneThread* self, RuneRuntimeThreadBody body, RuneString const* name);

/// Starts a named joinable OS thread after copying `body.context` into owned storage.
/// Used by backends for escaping captured closures.
void rune_runtime_thread_spawn_named_with_context(
    RuneThread* self,
    RuneRuntimeThreadBody body,
    RuneString const* name,
    usize contextSize,
    usize contextAlignment);

/// Waits for the thread to finish.
void rune_runtime_thread_join(RuneThread* self);

/// Releases the native thread handle without waiting.
void rune_runtime_thread_detach(RuneThread* self);

/// Releases thread storage during finalization.
void rune_runtime_thread_destroy(RuneThread* self);

/// Returns the platform-native thread handle for FFI escape hatches.
void* rune_runtime_thread_native_handle(RuneThread* self);

/// Writes the current thread name into `out` when one has been set through Runtime/CStd.
bool rune_runtime_thread_current_name(RuneString* out);

/// Sets the current platform thread's debugger-visible name.
void rune_runtime_thread_set_current_name(RuneString const* name);

/// Sleeps the current thread for at least `nanoseconds`.
void rune_runtime_thread_sleep_nanoseconds(int64 nanoseconds);

/// Sleeps the current thread until the monotonic instant identified by `nanoseconds`.
void rune_runtime_thread_sleep_until_nanoseconds(int64 nanoseconds);

/// Yields execution of the current thread to the platform scheduler.
void rune_runtime_thread_yield(void);

#ifdef __cplusplus
}
#endif
