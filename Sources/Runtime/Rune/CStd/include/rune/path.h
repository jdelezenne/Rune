#pragma once

#include <rune/string.h>
#include <rune/stringslice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Owned, UTF-8 file-system path.
///
/// Path components are always separated by '/'. Windows '\' separators are
/// normalized to '/' on construction and mutation. Passing a `RunePath` to
/// platform file APIs performs any further encoding conversion at that boundary
/// (e.g. UTF-8 → UTF-16 on Windows).
typedef struct RunePath
{
    RuneString data; ///< UTF-8 path bytes (not null-terminated).
} RunePath;

// MARK: - Lifetime

/// Returns an empty path.
/// @return Initialised empty path.
RunePath rune_path_create(void);

/// Releases the backing string storage.
/// @param self Path to tear down.
void rune_path_destroy(RunePath* self);

/// Deep-copies `source` into `destination`.
/// @param destination Destination path storage.
/// @param source Source path to copy.
void rune_path_copy(RunePath* destination, RunePath const* source);

// MARK: - Construction

/// Assigns from an owned UTF-8 string, replacing any existing content.
/// @param self Target path.
/// @param string Source UTF-8 string.
void rune_path_from_string(RunePath* self, RuneString const* string);

/// Assigns from a UTF-8 string slice, replacing any existing content.
/// @param self Target path.
/// @param slice Source UTF-8 string slice.
void rune_path_from_slice(RunePath* self, RuneStringSlice const* slice);

// MARK: - Queries

/// Returns true when the path has no bytes.
/// @param self Path to inspect.
/// @return `true` when empty.
bool rune_path_is_empty(RunePath const* self);

/// Returns true when the path is absolute (starts with '/' or a drive root on Windows).
/// @param self Path to inspect.
/// @return `true` when absolute.
bool rune_path_is_absolute(RunePath const* self);

/// Returns true when the path is relative.
/// @param self Path to inspect.
/// @return `true` when relative.
bool rune_path_is_relative(RunePath const* self);

// MARK: - Decomposition
// Returned slices are views into `self`; invalidated by any mutation.

/// Returns a slice over the entire path string.
/// @param self Path to inspect.
/// @return Slice over the raw path bytes.
RuneStringSlice rune_path_as_slice(RunePath const* self);

/// Returns a slice over the final path component (filename).
/// Empty when the path ends with '/'.
/// @param self Path to inspect.
/// @return Slice over the filename component.
RuneStringSlice rune_path_filename(RunePath const* self);

/// Returns a slice over the filename without its extension.
/// @param self Path to inspect.
/// @return Slice over the stem component.
RuneStringSlice rune_path_stem(RunePath const* self);

/// Returns a slice over the extension without the leading dot.
/// Empty when there is no extension.
/// @param self Path to inspect.
/// @return Slice over the extension, or empty.
RuneStringSlice rune_path_extension(RunePath const* self);

/// Writes the parent directory into `out`. `out` is empty when there is no parent.
/// @param self Path to inspect.
/// @param out Must be constructed by the caller before this call.
void rune_path_parent(RunePath const* self, RunePath* out);

// MARK: - Mutation

/// Appends `component`, inserting '/' between the existing path and the
/// component when needed. If `component` is absolute it replaces `self`.
/// @param self Target path.
/// @param component Component to append.
void rune_path_push(RunePath* self, RuneStringSlice const* component);

/// Appends all components of `other`, inserting '/' when needed.
/// @param self Target path.
/// @param other Path whose components are appended.
void rune_path_push_path(RunePath* self, RunePath const* other);

/// Replaces the extension. Pass an empty or null slice to remove the extension.
/// @param self Target path.
/// @param ext New extension without the leading dot, or empty/null to remove.
void rune_path_set_extension(RunePath* self, RuneStringSlice const* ext);

#ifdef __cplusplus
}
#endif
