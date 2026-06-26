#pragma once

#include <rune/platform/windows/platform.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuneProcess
{
    HANDLE process_handle;
    HANDLE thread_handle;
    bool   initialized;
} RuneProcess;

#ifdef __cplusplus
}
#endif
