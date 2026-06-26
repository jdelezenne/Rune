#pragma once

#include <cassert>
#include <cstdlib>
#include <exception>

namespace rune
{
    /// Terminates the process immediately.
    [[noreturn]] inline void abort() noexcept
    {
        std::abort();
    }

    /// Calls std::terminate.
    [[noreturn]] inline void terminate() noexcept
    {
        std::terminate();
    }
}

#if defined(_MSC_VER)
#define RUNE_DEBUG_BREAK() __debugbreak()
#else
#include <csignal>
#define RUNE_DEBUG_BREAK() std::raise(SIGTRAP)
#endif

/// Asserts that condition is true in all build configurations.
#define RUNE_ASSERT(condition) assert(condition)

/// Asserts that condition is true with an explanatory message.
#define RUNE_ASSERT_MSG(condition, message) assert((condition) && (message))

#ifndef NDEBUG
/// Debug-only assertion that condition is true.
#define RUNE_DEBUG_ASSERT(condition) assert(condition)

/// Debug-only assertion that condition is true with an explanatory message.
#define RUNE_DEBUG_ASSERT_MSG(condition, message) assert((condition) && (message))
#else
#define RUNE_DEBUG_ASSERT(condition) ((void)0)
#define RUNE_DEBUG_ASSERT_MSG(condition, message) ((void)0)
#endif

/// Unconditional assertion failure in all build configurations.
#define RUNE_ASSERT_FAIL() assert(false)

/// Unconditional assertion failure with an explanatory message.
#define RUNE_ASSERT_FAIL_MSG(message) assert(!"RUNE_ASSERT_FAIL_MSG")

#ifndef NDEBUG
/// Debug-only unconditional assertion failure.
#define RUNE_DEBUG_ASSERT_FAIL() assert(false)

/// Debug-only unconditional assertion failure with an explanatory message.
#define RUNE_DEBUG_ASSERT_FAIL_MSG(message) assert(!"RUNE_DEBUG_ASSERT_FAIL_MSG")
#else
#define RUNE_DEBUG_ASSERT_FAIL() ((void)0)
#define RUNE_DEBUG_ASSERT_FAIL_MSG(message) ((void)0)
#endif

/// Aborts the process on unrecoverable runtime errors.
#define RUNE_FATAL_ERROR() rune::abort()

/// Aborts the process on unrecoverable runtime errors with an explanatory message.
#define RUNE_FATAL_ERROR_MSG(message) rune::abort()

#if defined(_MSC_VER)
/// Marks a code path as unreachable and aborts if execution reaches it.
#define RUNE_UNREACHABLE() \
    do                     \
    {                      \
        rune::abort();     \
        __assume(0);       \
    }                      \
    while (0)
#elif defined(__clang__) || defined(__GNUC__)
/// Marks a code path as unreachable and aborts if execution reaches it.
#define RUNE_UNREACHABLE()       \
    do                           \
    {                            \
        rune::abort();           \
        __builtin_unreachable(); \
    }                            \
    while (0)
#else
/// Marks a code path as unreachable and aborts if execution reaches it.
#define RUNE_UNREACHABLE() rune::abort()
#endif
