#pragma once

#include <rune/path.h>
#include <rune/string.h>
#include <rune/time.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Identifies the type of a filesystem entry.
typedef enum RuneFileKind
{
    RuneFileKindFile = 0,      ///< Regular file.
    RuneFileKindDirectory = 1, ///< Directory.
    RuneFileKindSymlink = 2,   ///< Symbolic link (not followed).
    RuneFileKindOther = 3,     ///< Device, pipe, socket, or unknown.
} RuneFileKind;

/// Metadata for a filesystem entry.
typedef struct RuneFileMetadata
{
    RuneFileKind kind;     ///< Entry type.
    uint64 size;           ///< Size in bytes; 0 for non-regular files.
    bool readonly;         ///< True when the entry has no write permission for the owner.
    RuneSystemTime created;  ///< Creation (birth) time; zero when unavailable.
    RuneSystemTime modified; ///< Last-modified time.
    RuneSystemTime accessed; ///< Last-accessed time.
} RuneFileMetadata;

/// A single entry yielded by a directory iteration.
/// The caller owns `path` and must call `rune_path_dispose` when done.
typedef struct RuneDirectoryEntry
{
    RunePath path;     ///< Full path to this entry.
    RuneFileKind kind; ///< Entry type.
} RuneDirectoryEntry;

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#include <rune/platform/windows/filesystem.h>
#elif defined(__APPLE__)
#include <rune/platform/darwin/filesystem.h>
#else
#include <rune/platform/posix/filesystem.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - Queries

/// Returns true when any filesystem entry exists at `path`.
/// @param path The path to check.
/// @return True if the entry exists, false otherwise.
bool rune_filesystem_exists(RunePath const* path);

/// Returns true when `path` refers to a regular file.
/// @param path The path to check.
/// @return True if the entry is a regular file, false otherwise.
bool rune_filesystem_is_file(RunePath const* path);

/// Returns true when `path` refers to a directory.
/// @param path The path to check.
/// @return True if the entry is a directory, false otherwise.
bool rune_filesystem_is_directory(RunePath const* path);

/// Retrieves metadata for `path`. Does not follow symlinks.
/// @param path      The path to query.
/// @param out       Pointer to the metadata structure to receive the values.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_metadata(RunePath const* path, RuneFileMetadata* out, RuneString* out_error);

// MARK: - Mutations

/// Creates a directory at `path`. Fails if it already exists or a parent is missing.
/// @param path      The path of the directory to create.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_create_directory(RunePath const* path, RuneString* out_error);

/// Creates `path` and all missing ancestor directories.
/// @param path      The path of the directory structure to create.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_create_directory_all(RunePath const* path, RuneString* out_error);

/// Removes the file at `path`.
/// @param path      The path of the file to remove.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_remove_file(RunePath const* path, RuneString* out_error);

/// Removes the empty directory at `path`.
/// @param path      The path of the directory to remove.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_remove_directory(RunePath const* path, RuneString* out_error);

/// Renames (or moves) `from` to `to`. Replaces `to` if it already exists.
/// @param from      The source path.
/// @param to        The destination path.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_rename(RunePath const* from, RunePath const* to, RuneString* out_error);

/// Copies the file at `from` to `to`. Overwrites `to` if it already exists.
/// @param from      The source file path.
/// @param to        The destination file path.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_filesystem_copy_file(RunePath const* from, RunePath const* to, RuneString* out_error);

// MARK: - Directory iteration

/// Opens `path` for iteration.
/// @param self      Uninitialised directory iteration storage.
/// @param path      The path to the directory to iterate.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_directory_open(RuneDirectory* self, RunePath const* path, RuneString* out_error);

/// Reads the next entry, skipping "." and "..".
/// @param self      Pointer to the directory iterator.
/// @param out_entry Receives the entry. The caller must call `rune_directory_entry_dispose`
///                  on each successfully written entry.
/// @return True when an entry was written; false when exhausted or on error.
bool rune_directory_read(RuneDirectory* self, RuneDirectoryEntry* out_entry);

/// Closes the directory iterator and releases resources.
/// @param self Pointer to the directory iterator.
void rune_directory_close(RuneDirectory* self);

/// Destructs a `RuneDirectoryEntry` returned by `rune_directory_read`.
/// @param self Pointer to the entry to dispose.
void rune_directory_entry_destroy(RuneDirectoryEntry* self);

#ifdef __cplusplus
}
#endif
