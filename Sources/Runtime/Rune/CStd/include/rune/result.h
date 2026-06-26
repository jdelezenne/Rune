#pragma once

#include <rune/forward.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Result carrying either a success value `T` or an error value `E`.
///
/// `RuneResult` is an opaque handle to caller-owned storage. Because `T` and
/// `E` are parametric there is no single concrete struct definition. Callers
/// allocate `RUNE_RESULT_STORAGE_BYTES(ok_size, err_size)` bytes aligned to
/// `RUNE_MAX_ALIGN` and pass the size of each variant to construction calls.
///
/// Storage layout: discriminant tag at offset 0 (`uint8`, 0 = ok, 1 = error),
/// then `RUNE_MAX_ALIGN - 1` bytes of padding, then the value region large
/// enough to hold the larger of `T` or `E`. The fixed `RUNE_MAX_ALIGN` offset
/// ensures the value is correctly aligned for any type when the caller's buffer
/// is declared `alignas(RUNE_MAX_ALIGN)`.
///
/// ## Usage
///
///     alignas(RUNE_MAX_ALIGN) uint8_t bytes[RUNE_RESULT_STORAGE_BYTES(sizeof(int), sizeof(float))];
///     RuneResult* result = (RuneResult*)bytes;
///     rune_result_ok(result, &myValue, sizeof(int));
///     if (rune_result_is_ok(result)) { ... }

/// Byte length of inline storage for `Result<T, E>`.
#define RUNE_RESULT_STORAGE_BYTES(ok_size, err_size) \
    (RUNE_MAX_ALIGN + ((ok_size) > (err_size) ? (ok_size) : (err_size)))

/// Constructs a success result in `self` by copying `value`.
/// @param self    Result storage to initialise.
/// @param value   Pointer to the success value bytes to copy.
/// @param ok_size Size of the success value in bytes.
void rune_result_ok(RuneResult* self, void const* value, usize ok_size);

/// Constructs an error result in `self` by copying `error`.
/// @param self     Result storage to initialise.
/// @param error    Pointer to the error value bytes to copy.
/// @param err_size Size of the error value in bytes.
void rune_result_error(RuneResult* self, void const* error, usize err_size);

/// Returns whether `self` is the success case.
/// @param self Result storage to inspect.
/// @return `true` when the success case is active.
bool rune_result_is_ok(RuneResult const* self);

/// Returns whether `self` is the error case.
/// @param self Result storage to inspect.
/// @return `true` when the error case is active.
bool rune_result_is_error(RuneResult const* self);

/// Returns a pointer to the success value, or null when in the error case.
/// @param self Result storage to unwrap.
/// @return Pointer to the success value bytes, or null.
void* rune_result_unwrap(RuneResult* self);

/// Returns a pointer to the error value, or null when in the success case.
/// @param self Result storage to unwrap.
/// @return Pointer to the error value bytes, or null.
void* rune_result_unwrap_error(RuneResult* self);

/// Returns a pointer to the success value, or `default_value` when in the error case.
/// @param self          Result storage to inspect.
/// @param default_value Fallback pointer when in the error case.
/// @return Pointer to the success or fallback value bytes.
void* rune_result_unwrap_or(RuneResult* self, void const* default_value);

#ifdef __cplusplus
}
#endif
