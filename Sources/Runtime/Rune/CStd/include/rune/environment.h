#pragma once

#include <rune/forward.h>
#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Initializes process environment state from command-line arguments and paths.
/// @param argc Argument count from `main`.
/// @param argv Argument vector from `main`.
/// @param current_directory Current working directory path.
/// @param executable_path Path to the running executable.
void rune_environment_initialize(
    int32 argc,
    char const* const* argv,
    char const* current_directory,
    char const* executable_path);

/// Initializes process environment state from the current host process.
void rune_environment_initialize_current_process(void);

/// Returns command-line arguments passed to the process.
/// @return Copy of the argument array.
RuneArray rune_environment_get_arguments(void);

/// Returns the current working directory.
/// @return Current working directory.
RuneString rune_environment_get_current_directory(void);

/// Returns the executable path.
/// @return Executable path.
RuneString rune_environment_get_executable_path(void);

/// Writes a snapshot of environment variables into `out`.
/// @param out Destination hashmap.
void rune_environment_get_variables(RuneHashMap* out);

/// Looks up an environment variable by name.
/// @param name Variable name.
/// @param out_value Receives the value when present.
/// @return `true` when the variable is set.
bool rune_environment_get_variable(RuneString const* name, RuneString* out_value);

/// Sets an environment variable for the lifetime of the process.
/// @param name Variable name.
/// @param value Value to assign.
/// @param overwrite When `false`, does not overwrite an existing value.
void rune_environment_set_variable(RuneString const* name, RuneString const* value, bool overwrite);

/// Removes an environment variable from the process environment.
/// @param name Variable name to remove.
void rune_environment_unset_variable(RuneString const* name);

/// Returns the process name.
/// @return Process name.
RuneString rune_environment_get_process_name(void);

/// Returns the process identifier (PID).
/// @return Process ID.
int64 rune_environment_get_process_id(void);

/// Returns the host name.
/// @return Host name.
RuneString rune_environment_get_host_name(void);

/// Returns the operating system name.
/// @return Operating system name.
RuneString rune_environment_get_operating_system_name(void);

/// Returns the operating system version.
/// @return Operating system version.
RuneString rune_environment_get_operating_system_version(void);

/// Returns the number of logical processors available to the process.
/// @return Processor count.
int64 rune_environment_get_processor_count(void);

/// Returns total physical memory installed on the machine, in bytes.
/// @return Physical memory size.
uint64 rune_environment_get_physical_memory(void);

/// Returns the login name of the current user.
/// @return Login name.
RuneString rune_environment_get_user_name(void);

/// Returns the full display name of the current user.
/// @return Full display name.
RuneString rune_environment_get_full_user_name(void);

/// Changes the current working directory of the process.
/// @param path Target directory path.
/// @param out_error Receives an error message on failure.
/// @return `true` on success.
bool rune_environment_change_current_directory(RuneString const* path, RuneString* out_error);

#ifdef __cplusplus
}
#endif
