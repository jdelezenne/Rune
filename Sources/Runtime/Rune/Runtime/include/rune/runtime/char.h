#pragma once

#include <rune/char.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

intptr rune_runtime_char_code_point(RuneChar self);
intptr rune_runtime_char_utf8_length(RuneChar self);
int64 rune_runtime_char_hash(RuneChar self);
void rune_runtime_string_from_char(RuneString* out, RuneChar self);

#ifdef __cplusplus
}
#endif
