#pragma once

#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Which half of a full-duplex connection to shut down.
typedef enum RuneSocketShutdown
{
    RuneSocketShutdownRead = 0,  ///< Stop accepting further reads.
    RuneSocketShutdownWrite = 1, ///< Stop accepting further writes.
    RuneSocketShutdownBoth = 2,  ///< Shut down both directions.
} RuneSocketShutdown;

/// Transport protocol a socket uses.
typedef enum RuneSocketProtocol
{
    RuneSocketProtocolTcp = 0, ///< Reliable, connection-oriented byte stream (`SOCK_STREAM`).
    RuneSocketProtocolUdp = 1, ///< Connectionless datagrams (`SOCK_DGRAM`).
} RuneSocketProtocol;

/// Opaque, platform-portable storage for a resolved network address (IPv4 or IPv6).
///
/// Sized and aligned to hold any platform's native socket address structure;
/// callers never inspect the bytes directly.
typedef struct RuneSocketAddress
{
    alignas(RUNE_MAX_ALIGN) uint8 storage[128];
    uint32 length;
} RuneSocketAddress;

#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#include <rune/platform/windows/socket.h>
#else
#include <rune/platform/posix/socket.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Resolves `host`:`port` into an address suitable for `rune_socket_connect` or
/// `rune_socket_listen`.
/// @param host       UTF-8 hostname or IPv4/IPv6 literal. Pass null to resolve the
///                   wildcard "any address", for binding a listener on all interfaces.
/// @param port       Port number in host byte order.
/// @param out_address Receives the resolved address.
/// @param out_error  Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_address_resolve(char const* host, uint16 port, RuneSocketAddress* out_address, RuneString* out_error);

/// Formats `address` as "host:port" (IPv6 hosts are bracketed).
/// @param address    Address to format.
/// @param out_string Receives the formatted text.
/// @return True on success.
bool rune_socket_address_to_string(RuneSocketAddress const* address, RuneString* out_string);

/// Extracts the numeric host encoded in `address`.
/// @param address  Address to inspect.
/// @param out_host Receives the host text without a port.
/// @return True on success.
bool rune_socket_address_host(RuneSocketAddress const* address, RuneString* out_host);

/// Returns the port encoded in `address`, in host byte order.
/// @param address Address to inspect.
/// @return The port number, or 0 when `address` is null.
uint16 rune_socket_address_port(RuneSocketAddress const* address);

/// Opens a connection to `address`. For `RuneSocketProtocolTcp` this performs a full
/// handshake; for `RuneSocketProtocolUdp` this only records `address` as the default
/// peer for subsequent `rune_socket_send`/`rune_socket_receive` calls (no packets are
/// exchanged).
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param address   Remote address to connect to.
/// @param protocol  Transport protocol to use.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_connect(RuneSocket* self, RuneSocketAddress const* address, RuneSocketProtocol protocol, RuneString* out_error);

/// Resolves `host`:`port` and connects to it. Equivalent to `rune_socket_address_resolve`
/// followed by `rune_socket_connect`.
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param host      UTF-8 hostname or IPv4/IPv6 literal.
/// @param port      Port number in host byte order.
/// @param protocol  Transport protocol to use.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_connect_to_host(RuneSocket* self, char const* host, uint16 port, RuneSocketProtocol protocol, RuneString* out_error);

/// Creates a socket bound to `address` without listening. Used for UDP servers, which
/// receive datagrams via `rune_socket_receive_from` without a `listen`/`accept` step.
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param address   Local address to bind to.
/// @param protocol  Transport protocol to use.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_bind(RuneSocket* self, RuneSocketAddress const* address, RuneSocketProtocol protocol, RuneString* out_error);

/// Resolves `host`:`port` and binds to it without listening. Equivalent to
/// `rune_socket_address_resolve` followed by `rune_socket_bind`. Pass a null `host` to
/// bind the wildcard "any address" (all interfaces).
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param host      UTF-8 hostname or IPv4/IPv6 literal, or null for the wildcard address.
/// @param port      Port number in host byte order.
/// @param protocol  Transport protocol to use.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_bind_to_host(RuneSocket* self, char const* host, uint16 port, RuneSocketProtocol protocol, RuneString* out_error);

/// Creates a TCP listening socket bound to `address`. TCP only: UDP has no
/// listen/accept step, see `rune_socket_bind` and `rune_socket_receive_from`.
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param address   Local address to bind to.
/// @param backlog   Maximum length of the pending-connection queue.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_listen(RuneSocket* self, RuneSocketAddress const* address, int32 backlog, RuneString* out_error);

/// Resolves `host`:`port` and creates a TCP listening socket bound to it. Equivalent to
/// `rune_socket_address_resolve` followed by `rune_socket_listen`. Pass a null `host` to
/// bind the wildcard "any address" (all interfaces).
/// @param self      Uninitialised socket storage. No-op when null or already initialised.
/// @param host      UTF-8 hostname or IPv4/IPv6 literal, or null for the wildcard address.
/// @param port      Port number in host byte order.
/// @param backlog   Maximum length of the pending-connection queue.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_listen_on_host(RuneSocket* self, char const* host, uint16 port, int32 backlog, RuneString* out_error);

/// Accepts a pending connection on a listening socket.
/// @param self               Listening socket, previously initialised with `rune_socket_listen`.
/// @param out_accepted       Uninitialised socket storage; receives the accepted connection.
/// @param out_remote_address Receives the remote peer's address; may be null.
/// @param out_error          Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_accept(RuneSocket* self, RuneSocket* out_accepted, RuneSocketAddress* out_remote_address, RuneString* out_error);

/// Attempts to accept a pending connection without treating "no connection is
/// ready yet" as an error.
/// @param self               Listening socket, previously initialised with `rune_socket_listen`.
/// @param out_accepted       Uninitialised socket storage; receives the accepted connection.
/// @param out_remote_address Receives the remote peer's address; may be null.
/// @param out_would_block    Receives true when no connection is currently ready.
/// @param out_error          Receives an error message on failure; may be null.
/// @return True on success or would-block, false on real failure.
bool rune_socket_accept_available(
    RuneSocket* self,
    RuneSocket* out_accepted,
    RuneSocketAddress* out_remote_address,
    bool* out_would_block,
    RuneString* out_error);

/// Sends up to `size` bytes from `data`.
/// @param self             Connected socket.
/// @param data             Bytes to send.
/// @param size             Number of bytes available in `data`.
/// @param out_bytes_sent   Receives the number of bytes sent; may be null.
/// @param out_error        Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_send(RuneSocket* self, void const* data, usize size, usize* out_bytes_sent, RuneString* out_error);

/// Receives up to `size` bytes into `buffer`.
/// @param self                 Connected socket.
/// @param buffer               Buffer to receive into.
/// @param size                 Maximum number of bytes to receive.
/// @param out_bytes_received   Receives the number of bytes read; 0 signals the peer closed the connection.
/// @param out_error            Receives an error message on failure; may be null.
/// @return True on success (including orderly close).
bool rune_socket_receive(RuneSocket* self, void* buffer, usize size, usize* out_bytes_received, RuneString* out_error);

/// Attempts to receive bytes without treating "no bytes are ready yet" as an error.
/// @param self                 Connected socket.
/// @param buffer               Buffer to receive into.
/// @param size                 Maximum number of bytes to receive.
/// @param out_bytes_received   Receives the number of bytes read; 0 signals the peer closed the connection.
/// @param out_would_block      Receives true when no bytes are currently ready.
/// @param out_error            Receives an error message on failure; may be null.
/// @return True on success or would-block, false on real failure.
bool rune_socket_receive_available(
    RuneSocket* self,
    void* buffer,
    usize size,
    usize* out_bytes_received,
    bool* out_would_block,
    RuneString* out_error);

/// Sends up to `size` bytes from `data` to `address`. UDP only; does not require
/// `rune_socket_connect` and does not change the socket's connected-peer state.
/// @param self             Bound or unbound UDP socket (implicitly created on first use).
/// @param data             Bytes to send.
/// @param size             Number of bytes available in `data`.
/// @param address          Destination address.
/// @param out_bytes_sent   Receives the number of bytes sent; may be null.
/// @param out_error        Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_send_to(RuneSocket* self, void const* data, usize size, RuneSocketAddress const* address, usize* out_bytes_sent, RuneString* out_error);

/// Resolves `host`:`port` and sends up to `size` bytes from `data` to it. Equivalent to
/// `rune_socket_address_resolve` followed by `rune_socket_send_to`.
/// @param self             Bound or unbound UDP socket (implicitly created on first use).
/// @param data             Bytes to send.
/// @param size             Number of bytes available in `data`.
/// @param host             UTF-8 hostname or IPv4/IPv6 literal.
/// @param port             Port number in host byte order.
/// @param out_bytes_sent   Receives the number of bytes sent; may be null.
/// @param out_error        Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_send_to_host(RuneSocket* self, void const* data, usize size, char const* host, uint16 port, usize* out_bytes_sent, RuneString* out_error);

/// Receives up to `size` bytes into `buffer`, recording the sender's address. UDP only.
/// @param self                 Bound UDP socket, e.g. from `rune_socket_bind`.
/// @param buffer               Buffer to receive into.
/// @param size                 Maximum number of bytes to receive.
/// @param out_from_address     Receives the sender's address; may be null.
/// @param out_bytes_received   Receives the number of bytes read; may be null.
/// @param out_error            Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_receive_from(RuneSocket* self, void* buffer, usize size, RuneSocketAddress* out_from_address, usize* out_bytes_received, RuneString* out_error);

/// Shuts down one or both halves of the connection without closing the descriptor.
/// @param self      Connected socket.
/// @param how       Which half (or both) to shut down.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_shutdown(RuneSocket* self, RuneSocketShutdown how, RuneString* out_error);

/// Sets whether `self` performs blocking or non-blocking I/O. Non-blocking `connect`,
/// `accept`, `send`, and `receive` calls return immediately with an error (rather than
/// blocking) when the operation would otherwise wait.
/// @param self      Pointer to the socket.
/// @param enabled   True for non-blocking I/O, false for the default blocking behaviour.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_set_nonblocking(RuneSocket* self, bool enabled, RuneString* out_error);

/// Sets `SO_REUSEADDR`, allowing a listening socket to bind to an address still in
/// `TIME_WAIT` from a previous listener. `rune_socket_listen`/`rune_socket_listen_on_host`
/// already enable this by default; use this to opt out or to set it before `connect`.
/// @param self      Pointer to the socket.
/// @param enabled   True to allow address reuse.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_set_reuse_address(RuneSocket* self, bool enabled, RuneString* out_error);

/// Sets `TCP_NODELAY`, disabling Nagle's algorithm so small writes are sent immediately
/// instead of being coalesced.
/// @param self      Pointer to a connected TCP socket.
/// @param enabled   True to disable Nagle's algorithm.
/// @param out_error Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_set_nodelay(RuneSocket* self, bool enabled, RuneString* out_error);

/// Sets the maximum time a blocking `rune_socket_receive` call will wait for data.
/// @param self         Pointer to the socket.
/// @param milliseconds Timeout in milliseconds; 0 disables the timeout (wait indefinitely).
/// @param out_error    Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_set_receive_timeout(RuneSocket* self, uint32 milliseconds, RuneString* out_error);

/// Sets the maximum time a blocking `rune_socket_send` call will wait for buffer space.
/// @param self         Pointer to the socket.
/// @param milliseconds Timeout in milliseconds; 0 disables the timeout (wait indefinitely).
/// @param out_error    Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_set_send_timeout(RuneSocket* self, uint32 milliseconds, RuneString* out_error);

/// Returns whether `self` currently holds a live socket.
/// @param self Pointer to the socket.
/// @return True when initialized, false when null or uninitialized.
bool rune_socket_is_open(RuneSocket const* self);

/// Closes the socket handle.
/// @param self Socket to close. No-op when null or uninitialized.
void rune_socket_close(RuneSocket* self);

/// Returns the local address a socket is bound to.
/// @param self       Pointer to the socket.
/// @param out_address Receives the local address.
/// @param out_error  Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_local_address(RuneSocket const* self, RuneSocketAddress* out_address, RuneString* out_error);

/// Returns the address of the remote peer a connected socket is talking to.
/// @param self       Pointer to the socket.
/// @param out_address Receives the remote address.
/// @param out_error  Receives an error message on failure; may be null.
/// @return True on success.
bool rune_socket_remote_address(RuneSocket const* self, RuneSocketAddress* out_address, RuneString* out_error);

/// Returns the platform-native handle (`int` fd on POSIX, `SOCKET` on Windows),
/// cast to `void*`. Returns null when uninitialized.
/// @param self Pointer to the socket.
/// @return The native handle cast to `void*`, or null when uninitialized.
void* rune_socket_native_handle(RuneSocket const* self);

#ifdef __cplusplus
}
#endif
