#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int rune_errno(void);
void* rune_stdin(void);
void* rune_stdout(void);
void* rune_stderr(void);

#ifdef __cplusplus
}
#endif
