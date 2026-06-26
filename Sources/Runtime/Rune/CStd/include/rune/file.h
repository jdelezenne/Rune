#pragma once

#include <rune/path.h>
#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Bitflags that control how a file is opened. Combine with `|`.
typedef enum RuneFileOpenFlags : uint32
{
    RuneFileOpenRead = 1u << 0,      ///< Open for reading.
    RuneFileOpenWrite = 1u << 1,     ///< Open for writing.
    RuneFileOpenAppend = 1u << 2,    ///< All writes go to end of file; implies `RuneFileOpenWrite`.
    RuneFileOpenCreate = 1u << 3,    ///< Create the file when it does not exist; requires `RuneFileOpenWrite`.
    RuneFileOpenCreateNew = 1u << 4, ///< Fail when the file already exists; requires `RuneFileOpenWrite`.
    RuneFileOpenTruncate = 1u << 5,  ///< Truncate to zero bytes on open; requires `RuneFileOpenWrite`.
} RuneFileOpenFlags;

/// Reference point for a seek operation.
typedef enum RuneSeekFrom
{
    RuneSeekFromStart = 0,   ///< Offset from the beginning of the file.
    RuneSeekFromCurrent = 1, ///< Offset from the current position.
    RuneSeekFromEnd = 2,     ///< Offset from the end of the file.
} RuneSeekFrom;

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#include <rune/platform/windows/file.h>
#elif defined(__APPLE__)
#include <rune/platform/darwin/file.h>
#else
#include <rune/platform/posix/file.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Opens the file at `path` according to `flags`.
/// @param self      Uninitialised file storage. No-op when null or already initialised.
/// @param path      UTF-8 path to the file.
/// @param flags     Combination of `RuneFileOpen*` flags.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_open(RuneFile* self, RunePath const* path, RuneFileOpenFlags flags, RuneString* out_error);

/// Closes the file handle.
/// @param self File to close. No-op when null or uninitialized.
void rune_file_close(RuneFile* self);

/// Reads up to `size` bytes into `buffer`.
/// @param self             Pointer to the RuneFile.
/// @param buffer           Buffer to read bytes into.
/// @param size             Maximum number of bytes to read.
/// @param out_bytes_read   Receives the number of bytes read; 0 signals end-of-file.
/// @param out_error        Receives an error message on failure; may be null.
/// @return True on success (including end-of-file).
bool rune_file_read(RuneFile* self, void* buffer, usize size, usize* out_bytes_read, RuneString* out_error);

/// Writes `size` bytes from `data`.
/// @param self               Pointer to the RuneFile.
/// @param data               Pointer to the data to write.
/// @param size               Number of bytes to write.
/// @param out_bytes_written  Receives the number of bytes written.
/// @param out_error          Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_write(RuneFile* self, void const* data, usize size, usize* out_bytes_written, RuneString* out_error);

/// Flushes OS write buffers to the underlying storage.
/// @param self      Pointer to the RuneFile.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_flush(RuneFile* self, RuneString* out_error);

/// Moves the file position by `offset` relative to `from`.
/// @param self          Pointer to the RuneFile.
/// @param offset        The number of bytes to move.
/// @param from          The reference point for the seek.
/// @param out_position  Receives the resulting absolute byte offset; may be null.
/// @param out_error     Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_seek(RuneFile* self, int64 offset, RuneSeekFrom from, uint64* out_position, RuneString* out_error);

/// Returns the current absolute byte offset.
/// @param self          Pointer to the RuneFile.
/// @param out_position  Receives the position.
/// @param out_error     Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_tell(RuneFile const* self, uint64* out_position, RuneString* out_error);

/// Returns the file size in bytes.
/// @param self       Pointer to the RuneFile.
/// @param out_size   Receives the size.
/// @param out_error  Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_size(RuneFile const* self, uint64* out_size, RuneString* out_error);

/// Truncates or extends the file to exactly `size` bytes.
/// @param self       Pointer to the RuneFile.
/// @param size       The new size of the file in bytes.
/// @param out_error  Receives an error message on failure; may be null.
/// @return True on success.
bool rune_file_set_size(RuneFile* self, uint64 size, RuneString* out_error);

/// Returns the platform-native handle (`int` fd on Darwin, `HANDLE` on Windows),
/// cast to `void*`. Returns null when uninitialized.
/// @param self Pointer to the RuneFile.
/// @return The native handle cast to `void*`, or null when uninitialized.
void* rune_file_native_handle(RuneFile const* self);

#ifdef __cplusplus
}
#endif
