#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Signed duration stored as nanoseconds.
typedef struct RuneDuration
{
    int64 nanoseconds; ///< Duration in nanoseconds.
} RuneDuration;

/// Monotonic timestamp. The epoch is platform-defined and only useful for differences.
typedef struct RuneInstant
{
    int64 nanoseconds; ///< Monotonic nanoseconds.
} RuneInstant;

/// Wall-clock timestamp stored as nanoseconds since the Unix epoch.
typedef struct RuneWallTime
{
    int64 unix_nanoseconds; ///< Nanoseconds since the Unix epoch.
} RuneWallTime;

/// Creates a duration from seconds.
/// @param seconds Number of seconds.
/// @return The constructed duration.
RuneDuration rune_duration_from_seconds(int64 seconds);

/// Creates a duration from milliseconds.
/// @param milliseconds Number of milliseconds.
/// @return The constructed duration.
RuneDuration rune_duration_from_milliseconds(int64 milliseconds);

/// Creates a duration from microseconds.
/// @param microseconds Number of microseconds.
/// @return The constructed duration.
RuneDuration rune_duration_from_microseconds(int64 microseconds);

/// Creates a duration from nanoseconds.
/// @param nanoseconds Number of nanoseconds.
/// @return The constructed duration.
RuneDuration rune_duration_from_nanoseconds(int64 nanoseconds);

/// Returns the duration as nanoseconds.
/// @param duration The duration to convert.
/// @return Number of nanoseconds in the duration.
int64 rune_duration_as_nanoseconds(RuneDuration duration);

/// Adds two durations.
/// @param left First duration.
/// @param right Second duration.
/// @return The sum of the two durations.
RuneDuration rune_duration_add(RuneDuration left, RuneDuration right);

/// Subtracts `right` from `left`.
/// @param left The duration to subtract from.
/// @param right The duration to subtract.
/// @return The difference between the two durations.
RuneDuration rune_duration_subtract(RuneDuration left, RuneDuration right);

/// Compares two durations, returning -1, 0, or 1.
/// @param left First duration to compare.
/// @param right Second duration to compare.
/// @return -1 if left < right, 0 if left == right, 1 if left > right.
int8 rune_duration_compare(RuneDuration left, RuneDuration right);

/// Returns the current monotonic time.
/// @return The current monotonic instant.
RuneInstant rune_time_monotonic_now(void);

/// Returns the current wall-clock time.
/// @return The current wall-clock time.
RuneWallTime rune_time_wall_now(void);

/// Returns `end - start`.
/// @param start Starting instant.
/// @param end Ending instant.
/// @return The duration between the two instants.
RuneDuration rune_time_between(RuneInstant start, RuneInstant end);

/// Returns the elapsed monotonic time since `start`.
/// @param start Starting instant.
/// @return The duration elapsed since start.
RuneDuration rune_time_elapsed_since(RuneInstant start);

/// Sleeps the current thread for at least `duration`, unless interrupted by the platform.
/// @param duration The duration to sleep for.
void rune_time_sleep(RuneDuration duration);

/// Yields execution of the current thread to the platform scheduler.
void rune_time_yield(void);

#ifdef __cplusplus
}
#endif
