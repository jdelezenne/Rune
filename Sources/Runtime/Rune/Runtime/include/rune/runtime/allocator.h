#pragma once

#include <rune/types.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RuneRuntimeAllocatorRef
{
    void* field_0;
    void* field_1;
} RuneRuntimeAllocatorRef;

void rune_runtime_allocator_initialize_default(void* allocator);
void rune_runtime_allocator_load_default(RuneRuntimeAllocatorRef* out);
void rune_runtime_allocator_load_current(RuneRuntimeAllocatorRef* out);
bool rune_runtime_allocator_load_override(void* out_allocator);
void rune_runtime_allocator_store_current(void const* allocator);
void rune_runtime_allocator_clear_current(void);

#ifdef __cplusplus
}
#endif
