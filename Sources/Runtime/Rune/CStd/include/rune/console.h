#pragma once

#include <rune/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Writes `value` to standard output without a trailing newline.
/// @param value String to print.
void rune_console_print(RuneString const* value);

/// Writes `value` to standard output followed by a newline.
/// @param value String to print.
void rune_console_print_line(RuneString const* value);

/// Reads one line from standard input.
/// @return The line read, without the trailing newline.
RuneString rune_console_read_line(void);

#ifdef __cplusplus
}
#endif
