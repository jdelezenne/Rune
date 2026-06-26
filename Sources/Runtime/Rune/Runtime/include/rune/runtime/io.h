#pragma once

#include <rune/path.h>
#include <rune/string.h>
#include <rune/stringslice.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// CStd's path.h returns RuneStringSlice by C value (`RuneStringSlice
/// rune_path_as_slice(RunePath const* self)`), but the Rune runtime ABI for
/// `@runtime` functions returning an aggregate always uses the
/// `(Aggregate* out, Self const* self, ...)` shape (confirmed by every
/// `rune_runtime_string_from_*` function and by `rune_cstring_as_string_slice`
/// in cstring.h). These reshape path.h's plain-return functions into that
/// shape; they do not add any behavior of their own.
void rune_runtime_path_as_slice(RuneStringSlice* out, RunePath const* self);
void rune_runtime_path_filename(RuneStringSlice* out, RunePath const* self);
void rune_runtime_path_stem(RuneStringSlice* out, RunePath const* self);
void rune_runtime_path_extension(RuneStringSlice* out, RunePath const* self);

/// Returns an owned copy of the path as a String.
void rune_runtime_path_to_string(RuneString* out, RunePath const* self);

/// Replaces `self` with an owned copy of `source`.
void rune_runtime_path_copy(RunePath* self, RunePath const* source);

/// Appends all components of `other`, inserting '/' when needed.
void rune_runtime_path_push_path(RunePath* self, RunePath const* other);

/// Returns whether two paths contain the same normalized bytes.
bool rune_runtime_path_equal(RunePath const* lhs, RunePath const* rhs);

/// Writes the parent directory into `out`.
bool rune_runtime_path_parent(RunePath const* self, RunePath* out);

#ifdef __cplusplus
}
#endif
