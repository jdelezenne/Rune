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

/// Advances `self` and returns a pointer to the yielded value stored in
/// `self->last_value`, or null when the range is exhausted. `rune_range_iterator_next`
/// itself already advances `self->current` past the yielded value as part of
/// computing it, so the yielded value cannot be read back from `self->current`
/// afterward - it needs its own field.
int64 const* rune_runtime_range_iterator_next(RuneRangeIterator* self);

#ifdef __cplusplus
}
#endif
