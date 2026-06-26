#pragma once

#include <rune/range.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_runtime_range_iterator_create(RuneRangeIterator* out, RuneRange const* self);

void rune_runtime_range_inclusive_iterator_create(RuneRangeIterator* out, RuneRange const* self);

void rune_runtime_range_to_iterator_create(RuneRangeIterator* out, RuneRange const* self);

void rune_runtime_range_from_iterator_create(RuneRangeIterator* out, RuneRange const* self);

void rune_runtime_range_to_inclusive_iterator_create(RuneRangeIterator* out, RuneRange const* self);

void rune_runtime_range_iterator_create_with_bounds(
    RuneRangeIterator* out,
    int64 start,
    int64 end,
    bool inclusive);

#ifdef __cplusplus
}
#endif
