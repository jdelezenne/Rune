#pragma once

#include <rune/rune.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_emscripten_set_main_loop(
    RuneFunction callback,
    int32 fps,
    int32 simulate_infinite_loop);

void rune_emscripten_cancel_main_loop(void);

#ifdef __cplusplus
}
#endif
