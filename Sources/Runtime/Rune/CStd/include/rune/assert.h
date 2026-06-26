#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuneString RuneString;

// MARK: - Panic handler

/// Called by rune_panic_internal before aborting. Must not return;
/// if it does, abort() is called unconditionally.
typedef void (*RunePanicHandler)(char const* message, char const* file, int line);

/// Replaces the global panic handler. Pass NULL to restore the default
/// (prints to stderr then aborts). Thread-safe.
void rune_set_panic_handler(RunePanicHandler handler);

/// Low-level entry point invoked by all panic macros. Prefer rune_panic().
[[noreturn]] void rune_panic_internal(char const* message, char const* file, int line);

/// Low-level entry point invoked by assertion macros.
[[noreturn]] void rune_assertion_failure_internal(char const* message, char const* file, int line);

/// Panic from Rune bytecode using string values for message and file path.
[[noreturn]] void rune_panic_rune(RuneString* message, RuneString* file, int32 line);

/// Assertion failure from Rune bytecode using string values for message and file path.
[[noreturn]] void rune_assertion_failure_rune(RuneString* message, RuneString* file, int32 line);

#ifdef __cplusplus
}
#endif

// MARK: - Macros

/// Unconditional panic with a message. Always active regardless of NDEBUG.
#define rune_panic(msg) rune_panic_internal(msg, __FILE__, __LINE__)

/// Hardware trap / software breakpoint. Stops in a debugger; terminates otherwise.
#if defined(_MSC_VER)
#define rune_trap() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define rune_trap() __builtin_trap()
#else
#define rune_trap() rune_panic_internal("trap", __FILE__, __LINE__)
#endif

/// Marks a code path as logically impossible.
/// Debug: panics with file and line. Release: optimizer hint (UB if reached).
#ifndef NDEBUG
#define rune_unreachable() rune_panic_internal("unreachable", __FILE__, __LINE__)
#elif defined(_MSC_VER)
#define rune_unreachable() __assume(false)
#elif defined(__GNUC__) || defined(__clang__)
#define rune_unreachable() __builtin_unreachable()
#else
#define rune_unreachable() ((void)0)
#endif

/// Runtime assertion. Disabled in release builds (NDEBUG defined).
#ifndef NDEBUG
#define rune_assert(cond) ((cond) ? (void)0 : rune_assertion_failure_internal("assertion failed: " #cond, __FILE__, __LINE__))
#define rune_assert_msg(cond, msg) ((cond) ? (void)0 : rune_assertion_failure_internal(msg, __FILE__, __LINE__))
#else
#define rune_assert(cond) ((void)(cond))
#define rune_assert_msg(cond, msg) ((void)(cond))
#endif
