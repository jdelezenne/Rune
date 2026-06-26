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

/// System clock timestamp stored as nanoseconds since the Unix epoch.
///
/// This is wall-clock time and may move backward or forward if the system clock
/// is adjusted. Use `RuneInstant` for monotonic measurements and deadlines.
typedef struct RuneSystemTime
{
    int64 unix_nanoseconds; ///< Nanoseconds since the Unix epoch.
} RuneSystemTime;

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

/// Creates a system time from seconds since the Unix epoch.
/// @param seconds Seconds since 1970-01-01 00:00:00 UTC.
/// @return The constructed system time.
RuneSystemTime rune_system_time_from_unix_seconds(double seconds);

/// Returns system-clock seconds since the Unix epoch.
/// @param time The system time to convert.
/// @return Seconds since 1970-01-01 00:00:00 UTC.
double rune_system_time_unix_seconds(RuneSystemTime time);

/// Adds seconds to a system time.
/// @param time The base system time.
/// @param seconds Seconds to add.
/// @return The adjusted system time.
RuneSystemTime rune_system_time_add_seconds(RuneSystemTime time, double seconds);

/// Returns `end - start` in seconds.
/// @param start Starting system time.
/// @param end Ending system time.
/// @return Seconds between the two system times.
double rune_system_time_seconds_between(RuneSystemTime start, RuneSystemTime end);

/// Compares two system times, returning -1, 0, or 1.
/// @param left First system time to compare.
/// @param right Second system time to compare.
/// @return -1 if left < right, 0 if left == right, 1 if left > right.
int8 rune_system_time_compare(RuneSystemTime left, RuneSystemTime right);

/// Creates Unix-epoch nanoseconds from seconds.
/// @param seconds Seconds since 1970-01-01 00:00:00 UTC.
/// @return Nanoseconds since 1970-01-01 00:00:00 UTC.
int64 rune_system_time_unix_nanoseconds_from_seconds(double seconds);

/// Converts Unix-epoch nanoseconds to seconds.
/// @param nanoseconds Nanoseconds since 1970-01-01 00:00:00 UTC.
/// @return Seconds since 1970-01-01 00:00:00 UTC.
double rune_system_time_unix_seconds_from_nanoseconds(int64 nanoseconds);

/// Returns the current system time as Unix-epoch nanoseconds.
/// @return Nanoseconds since 1970-01-01 00:00:00 UTC.
int64 rune_system_time_now_unix_nanoseconds(void);

/// Returns the current monotonic time.
/// @return The current monotonic instant.
RuneInstant rune_instant_now(void);

/// Returns the current system-clock time.
/// @return The current system-clock time.
RuneSystemTime rune_system_time_now(void);

/// Returns `end - start`.
/// @param start Starting instant.
/// @param end Ending instant.
/// @return The duration between the two instants.
RuneDuration rune_instant_duration_since(RuneInstant start, RuneInstant end);

/// Returns the elapsed monotonic time since `start`.
/// @param start Starting instant.
/// @return The duration elapsed since start.
RuneDuration rune_instant_elapsed_since(RuneInstant start);

#ifdef __cplusplus
}
#endif
