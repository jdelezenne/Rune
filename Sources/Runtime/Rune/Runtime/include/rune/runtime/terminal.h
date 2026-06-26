#pragma once

#include <rune/string.h>
#include <rune/types.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool rune_runtime_terminal_is_interactive(void);

bool rune_runtime_terminal_size(uint32* out_columns, uint32* out_rows, RuneString* out_error);

bool rune_runtime_terminal_enable_raw_mode(RuneString* out_error);

void rune_runtime_terminal_disable_raw_mode(void);

bool rune_runtime_terminal_enter_alternate_screen(RuneString* out_error);

void rune_runtime_terminal_leave_alternate_screen(void);

void rune_runtime_terminal_clear(void);

void rune_runtime_terminal_move_cursor(uint32 row, uint32 column);

void rune_runtime_terminal_set_cursor_visible(bool visible);

void rune_runtime_terminal_write(RuneString const* value);

bool rune_runtime_terminal_read_available(RuneString* out, RuneString* out_error);

#ifdef __cplusplus
}
#endif
