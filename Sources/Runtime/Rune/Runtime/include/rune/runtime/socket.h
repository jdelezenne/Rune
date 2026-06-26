#pragma once

#include <rune/socket.h>
#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Zero-initializes an unconnected, unbound socket.
void rune_runtime_socket_create(RuneSocket* self);

/// Opens a TCP connection to `host`:`port`.
/// @param out_error Receives an error message on failure; the caller-owned String is
///                   left untouched on success.
/// @return True on success; failure leaves `self` uninitialized.
bool rune_runtime_socket_connect(RuneSocket* self, RuneString const* host, uint16 port, RuneString* out_error);

/// Binds and listens on `host`:`port`.
/// @param out_error Receives an error message on failure; the caller-owned String is
///                   left untouched on success.
/// @return True on success; failure leaves `self` uninitialized.
bool rune_runtime_socket_listen(RuneSocket* self, RuneString const* host, uint16 port, int32 backlog, RuneString* out_error);

/// Accepts the next pending connection on `self`, blocking until one arrives.
/// `out` is left uninitialized on failure.
/// @param out_error Receives an error message on failure; the caller-owned String is
///                   left untouched on success.
void rune_runtime_socket_accept(RuneSocket* out, RuneSocket* self, RuneString* out_error);

/// Attempts to accept a connection. `out_would_block` receives true when no
/// connection is ready on a non-blocking listener.
void rune_runtime_socket_accept_available(
    RuneSocket* out,
    RuneSocket* self,
    bool* out_would_block,
    RuneString* out_error);

/// Sends up to `length` bytes starting at `data`.
/// @param out_error Receives an error message when the result is -1; left untouched otherwise.
/// @return The number of bytes written, or -1 on failure.
int64 rune_runtime_socket_send(RuneSocket* self, void const* data, usize length, RuneString* out_error);

/// Reads up to `length` bytes into `buffer`.
/// @param out_error Receives an error message when the result is -1; left untouched otherwise.
/// @return The number of bytes read, 0 at end of stream, or -1 on failure.
int64 rune_runtime_socket_receive(RuneSocket* self, void* buffer, usize length, RuneString* out_error);

/// Attempts to receive bytes. Returns true for both data/close and would-block;
/// `out_would_block` distinguishes the latter.
bool rune_runtime_socket_receive_available(
    RuneSocket* self,
    void* buffer,
    usize length,
    uint64* out_received,
    bool* out_would_block,
    RuneString* out_error);

/// Binds `self` for UDP without listening.
/// @param out_error Receives an error message on failure; left untouched on success.
/// @return True on success; failure leaves `self` uninitialized.
bool rune_runtime_socket_bind(RuneSocket* self, RuneString const* host, uint16 port, RuneString* out_error);

/// Sends up to `length` bytes to `host`:`port` as a UDP datagram. Implicitly creates the
/// underlying socket on first use.
/// @param out_error Receives an error message when the result is -1; left untouched otherwise.
/// @return The number of bytes sent, or -1 on failure.
int64 rune_runtime_socket_send_to(RuneSocket* self, void const* data, usize length, RuneString const* host, uint16 port, RuneString* out_error);

/// Receives up to `length` bytes into `buffer` as a UDP datagram, returning the sender's
/// address as separate host and port values.
/// @param out_from_host Receives the sender's host; left as an empty string on failure.
/// @param out_from_port Receives the sender's port; left as 0 on failure.
/// @param out_error     Receives an error message when the result is -1; left untouched otherwise.
/// @return The number of bytes read, or -1 on failure.
int64 rune_runtime_socket_receive_from(
    RuneSocket* self,
    void* buffer,
    usize length,
    RuneString* out_from_host,
    uint16* out_from_port,
    RuneString* out_error);

/// Formats the local address `self` is bound to as "host:port".
/// Writes an empty string when the address cannot be determined.
void rune_runtime_socket_local_address(RuneString* out, RuneSocket* self);

/// Formats the remote peer address `self` is connected to as "host:port".
/// Writes an empty string when the address cannot be determined.
void rune_runtime_socket_remote_address(RuneString* out, RuneSocket* self);

/// Shuts down both halves of the connection without closing the descriptor.
/// Drops the richer `rune_socket_shutdown` direction/error
/// parameters that don't need Rune ABI exposure.
bool rune_runtime_socket_shutdown(RuneSocket* self);

/// Sets whether `self` performs blocking or non-blocking I/O.
bool rune_runtime_socket_set_nonblocking(RuneSocket* self, bool enabled);

/// Sets `SO_REUSEADDR`.
bool rune_runtime_socket_set_reuse_address(RuneSocket* self, bool enabled);

/// Sets `TCP_NODELAY`.
bool rune_runtime_socket_set_nodelay(RuneSocket* self, bool enabled);

/// Sets the receive timeout in milliseconds (0 disables it).
bool rune_runtime_socket_set_receive_timeout(RuneSocket* self, uint32 milliseconds);

/// Sets the send timeout in milliseconds (0 disables it).
bool rune_runtime_socket_set_send_timeout(RuneSocket* self, uint32 milliseconds);

#ifdef __cplusplus
}
#endif
