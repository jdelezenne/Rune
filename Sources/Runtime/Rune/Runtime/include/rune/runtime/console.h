#pragma once

#include <rune/string.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_runtime_console_print(RuneString value);

void rune_runtime_console_print_line(RuneString value);

void rune_runtime_console_read_line(RuneString* out);

#ifdef __cplusplus
}
#endif
