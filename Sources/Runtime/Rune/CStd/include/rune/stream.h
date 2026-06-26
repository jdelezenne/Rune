#pragma once

#include <rune/file.h>
#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - Status

/// Describes the lifecycle state of a stream.
typedef enum RuneStreamStatus
{
    RuneStreamStatusNotOpen = 0, ///< Stream has not been opened.
    RuneStreamStatusOpen = 1,    ///< Stream is open and ready for I/O.
    RuneStreamStatusAtEnd = 2,   ///< All bytes have been consumed (input only).
    RuneStreamStatusClosed = 3,  ///< Stream has been closed.
    RuneStreamStatusError = 4,   ///< An unrecoverable error occurred.
} RuneStreamStatus;

// MARK: - Input stream

/// Opaque per-instance state for a `RuneInputStream` implementation.
///
/// Each implementation defines its own concrete struct and casts to/from
/// `RuneInputStreamContext*` inside its callbacks. Callers never dereference
/// this pointer directly.
typedef struct RuneInputStreamContext RuneInputStreamContext;

/// Interface for a readable byte stream.
///
/// To implement a custom readable stream, fill a `RuneInputStreamInterface`,
/// allocate your concrete context on the heap, and store both in a
/// `RuneInputStream`. The `close` callback must free the context.
typedef struct RuneInputStreamInterface
{
    /// Reads up to `size` bytes into `buffer`.
    /// Returns the number of bytes read, 0 at end of stream, or negative on error.
    int64 (*read)(RuneInputStreamContext* context, void* buffer, usize size, RuneString* out_error);

    /// Releases resources. Called exactly once by `rune_input_stream_close`;
    /// must free `context`.
    void (*close)(RuneInputStreamContext* context);

    /// Returns the current stream status.
    RuneStreamStatus (*status)(RuneInputStreamContext const* context);
} RuneInputStreamInterface;

/// A readable byte stream.
///
/// `interface` and `context` are set by the concrete open function
/// (e.g. `rune_file_input_stream_open`). Callers operate exclusively
/// through `rune_input_stream_*` functions.
typedef struct RuneInputStream
{
    RuneInputStreamInterface const* interface; ///< Operation dispatch interface.
    RuneInputStreamContext* context;           ///< Heap-allocated implementation context.
} RuneInputStream;

// MARK: - Output stream

/// Opaque per-instance state for a `RuneOutputStream` implementation.
///
/// Each implementation defines its own concrete struct and casts to/from
/// `RuneOutputStreamContext*` inside its callbacks. Callers never dereference
/// this pointer directly.
typedef struct RuneOutputStreamContext RuneOutputStreamContext;

/// Interface for a writable byte stream.
///
/// To implement a custom writable stream, fill a `RuneOutputStreamInterface`,
/// allocate your concrete context on the heap, and store both in a
/// `RuneOutputStream`. The `close` callback must free the context.
typedef struct RuneOutputStreamInterface
{
    /// Writes `size` bytes from `data`.
    /// Returns the number of bytes written, or negative on error.
    int64 (*write)(RuneOutputStreamContext* context, void const* data, usize size, RuneString* out_error);

    /// Flushes any buffered bytes to the underlying sink.
    /// Returns true on success.
    bool (*flush)(RuneOutputStreamContext* context, RuneString* out_error);

    /// Releases resources. Called exactly once by `rune_output_stream_close`;
    /// must free `context`.
    void (*close)(RuneOutputStreamContext* context);

    /// Returns the current stream status.
    RuneStreamStatus (*status)(RuneOutputStreamContext const* context);
} RuneOutputStreamInterface;

/// A writable byte stream.
///
/// `interface` and `context` are set by the concrete open function
/// (e.g. `rune_file_output_stream_open`). Callers operate exclusively
/// through `rune_output_stream_*` functions.
typedef struct RuneOutputStream
{
    RuneOutputStreamInterface const* interface; ///< Operation dispatch interface.
    RuneOutputStreamContext* context;           ///< Heap-allocated implementation context.
} RuneOutputStream;

// MARK: - RuneInputStream operations

/// Reads up to `size` bytes from `stream` into `buffer`.
/// @param stream             Pointer to the input stream.
/// @param buffer           Pointer to the destination buffer.
/// @param size             Maximum number of bytes to read.
/// @param out_bytes_read   Receives the number of bytes read; 0 signals end of stream.
/// @param out_error        Receives an error message on failure; may be null.
/// @return True on success (including end of stream).
bool rune_input_stream_read(RuneInputStream* stream, void* buffer, usize size, usize* out_bytes_read, RuneString* out_error);

/// Closes the stream and releases all resources.
/// After this call `stream->interface` and `stream->context` are null.
/// @param stream Pointer to the input stream to close.
void rune_input_stream_close(RuneInputStream* stream);

/// Returns the current status of the stream.
/// @param stream Pointer to the input stream.
/// @return The current RuneStreamStatus of the stream.
RuneStreamStatus rune_input_stream_status(RuneInputStream const* stream);

/// Returns true when the stream has reached end of input.
/// @param stream Pointer to the input stream.
/// @return True if the stream is at the end of input, false otherwise.
bool rune_input_stream_is_at_end(RuneInputStream const* stream);

// MARK: - RuneOutputStream operations

/// Writes `size` bytes from `data` into `stream`.
/// @param stream               Pointer to the output stream.
/// @param data               Pointer to the source byte data.
/// @param size               The number of bytes to write.
/// @param out_bytes_written  Receives the number of bytes written; may be null.
/// @param out_error          Receives an error message on failure; may be null.
/// @return True on success.
bool rune_output_stream_write(RuneOutputStream* stream, void const* data, usize size, usize* out_bytes_written, RuneString* out_error);

/// Flushes any internally buffered bytes to the underlying sink.
/// @param stream      Pointer to the output stream.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_output_stream_flush(RuneOutputStream* stream, RuneString* out_error);

/// Closes the stream and releases all resources.
/// After this call `stream->interface` and `stream->context` are null.
/// @param stream Pointer to the output stream to close.
void rune_output_stream_close(RuneOutputStream* stream);

/// Returns the current status of the stream.
/// @param stream Pointer to the output stream.
/// @return The current RuneStreamStatus of the stream.
RuneStreamStatus rune_output_stream_status(RuneOutputStream const* stream);

// MARK: - File-backed streams

/// Opens `file` for reading through `stream`. Does not take ownership of `file`;
/// the caller must keep it open for the lifetime of `stream` and close it separately.
/// @param stream   Uninitialised stream storage.
/// @param file   An open, readable `RuneFile`.
void rune_file_input_stream_open(RuneInputStream* stream, RuneFile* file);

/// Opens `file` for writing through `stream`. Does not take ownership of `file`;
/// the caller must keep it open for the lifetime of `stream` and close it separately.
/// @param stream   Uninitialised stream storage.
/// @param file   An open, writable `RuneFile`.
void rune_file_output_stream_open(RuneOutputStream* stream, RuneFile* file);

// MARK: - Memory-backed streams

/// Opens a read-only stream over the byte range `[data, data + size)`.
/// Does not take ownership of the buffer; the caller must ensure it remains
/// valid for the lifetime of `stream`.
/// @param stream   Uninitialised stream storage.
/// @param data   Buffer to read from.
/// @param size   Number of bytes in the buffer.
void rune_memory_input_stream_open(RuneInputStream* stream, void const* data, usize size);

/// Opens a write-only stream backed by an internally managed, growable buffer.
/// Use `rune_memory_output_stream_bytes` to access the accumulated data before closing.
/// @param stream Uninitialised stream storage.
void rune_memory_output_stream_open(RuneOutputStream* stream);

/// Returns a pointer and size for the bytes written so far.
/// The pointer is valid until the next write or until `rune_output_stream_close` is called.
/// @param stream     A stream opened with `rune_memory_output_stream_open`.
/// @param out_data Receives a pointer to the internal buffer; may be null on entry.
/// @param out_size Receives the number of bytes written; may be null on entry.
/// @return True when `stream` is a memory output stream and values were written.
bool rune_memory_output_stream_bytes(RuneOutputStream const* stream, void const** out_data, usize* out_size);

#ifdef __cplusplus
}
#endif
