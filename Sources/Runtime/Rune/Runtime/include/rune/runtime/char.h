#pragma once

#include <rune/char.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

int64 rune_runtime_char_code_point(RuneChar self);
int64 rune_runtime_char_utf8_length(RuneChar self);
int64 rune_runtime_char_hash(RuneChar self);

#ifdef __cplusplus
}
#endif
