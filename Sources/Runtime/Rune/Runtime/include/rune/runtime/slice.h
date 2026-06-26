#pragma once

#include <rune/slice.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_runtime_slice_initialize(RuneSlice* out, usize element_size);

void rune_runtime_slice_borrow(
    RuneSlice* out,
    void const* elements,
    intptr count,
    usize element_size);

void rune_runtime_slice_sub(RuneSlice* out, RuneSlice const* self, intptr start, intptr end);

void rune_runtime_slice_iterator_create(RuneContiguousIterator* out, RuneSlice const* self);

#ifdef __cplusplus
}
#endif
