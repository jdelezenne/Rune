#pragma once

#include <rune/path.h>
#include <rune/string.h>
#include <rune/stringslice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Configuration for spawning a child process.
typedef struct RuneProcessConfig
{
    RunePath const*        executable;        ///< Path to the executable.
    RuneStringSlice const* arguments;         ///< Additional arguments (argv[1..]). May be null when `argument_count` is 0.
    usize                  argument_count;
    RuneStringSlice const* environment;       ///< Environment as UTF-8 "KEY=VALUE" pairs. Null inherits the parent environment.
    usize                  environment_count;
    RunePath const*        working_directory; ///< Initial working directory. Null inherits the parent working directory.
} RuneProcessConfig;

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#include <rune/platform/windows/process.h>
#else
#include <rune/platform/posix/process.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - Lifetime

// MARK: - Spawning

/// Spawns a new child process according to `config`.
/// @param self      Uninitialised process storage.
/// @param config    Spawn configuration. `config->executable` must not be null.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_process_spawn(RuneProcess* self, RuneProcessConfig const* config, RuneString* out_error);

// MARK: - Control

/// Sends an unconditional termination signal to the process.
/// On POSIX this sends `SIGKILL`; on Windows it calls `TerminateProcess`.
/// Does not wait for the process to exit.
/// @return True on success.
bool rune_process_kill(RuneProcess* self, RuneString* out_error);

// MARK: - Observation

/// Blocks until the process exits and returns its exit code.
/// After a successful wait the handle is consumed; further calls are no-ops.
/// @param out_exit_code Receives the exit code. On POSIX, signal-terminated
///                      processes yield a negative value equal to `-signal`.
/// @return True on success.
bool rune_process_wait(RuneProcess* self, int32* out_exit_code, RuneString* out_error);

/// Returns the OS process identifier, or 0 when not initialised.
uint64 rune_process_id(RuneProcess const* self);

/// Releases OS handles without waiting for the process to exit.
/// On POSIX, failure to call `rune_process_wait` before `rune_process_close`
/// will leave a zombie until the parent process exits.
void rune_process_close(RuneProcess* self);

#ifdef __cplusplus
}
#endif
