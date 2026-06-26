#pragma once

#include <rune/array.h>
#include <rune/environment.h>
#include <rune/path.h>
#include <rune/string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// CStd's environment.h returns these values by C value (e.g. `RuneString
/// rune_environment_get_current_directory(void)`), but the Rune runtime ABI
/// for `@runtime` functions returning an aggregate always uses the
/// `(Aggregate* out, ...)` shape (confirmed by every `rune_runtime_string_from_*`
/// function and by `rune_runtime_path_to_string` in io.h). These reshape
/// environment.h's plain-return functions into that shape; they do not add
/// any behavior of their own.
void rune_runtime_environment_get_arguments(RuneArray* out);
void rune_runtime_environment_get_current_directory(RuneString* out);
void rune_runtime_environment_get_temporary_directory(RuneString* out);
void rune_runtime_environment_get_executable_path(RuneString* out);
void rune_runtime_application_get_resources_directory(RunePath* out);
void rune_runtime_environment_get_process_name(RuneString* out);
void rune_runtime_environment_get_host_name(RuneString* out);
void rune_runtime_environment_get_operating_system_name(RuneString* out);
void rune_runtime_environment_get_operating_system_version(RuneString* out);
void rune_runtime_environment_get_user_name(RuneString* out);
void rune_runtime_environment_get_full_user_name(RuneString* out);

#ifdef __cplusplus
}
#endif
