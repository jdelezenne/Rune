#pragma once

#include <rune/types.h>

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuneProcess
{
    pid_t pid;
    bool  initialized;
} RuneProcess;

#ifdef __cplusplus
}
#endif
