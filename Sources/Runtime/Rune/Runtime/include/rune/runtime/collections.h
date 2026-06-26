#pragma once

#include <rune/array.h>
#include <rune/buffer.h>
#include <rune/hashmap.h>
#include <rune/hashset.h>
#include <rune/inlinearray.h>
#include <rune/slice.h>
#include <rune/sortedmap.h>
#include <rune/sortedset.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RunePackExpansion
{
    RuneBuffer buffer;
    intptr count;
} RunePackExpansion;

void rune_runtime_buffer_initialize(RuneBuffer* self, usize element_size);

intptr rune_runtime_buffer_capacity(RuneBuffer const* self);

void rune_runtime_buffer_reserve(RuneBuffer* self, intptr count);

void const* rune_runtime_buffer_get(RuneBuffer const* self, intptr index);

void rune_runtime_buffer_set(RuneBuffer* self, intptr index, void const* element);

void rune_runtime_buffer_move_elements(
    RuneBuffer* self,
    intptr dst_index,
    intptr src_index,
    intptr count);

void rune_runtime_buffer_copy(
    RuneBuffer* self,
    RuneBuffer const* src,
    intptr count);

bool rune_runtime_buffer_contains(RuneBuffer const* self, intptr count, void const* element);

intptr rune_runtime_buffer_index_of(RuneBuffer const* self, intptr count, void const* element);

void rune_runtime_array_initialize(RuneArray* self, usize element_size);

void rune_runtime_array_initialize_with_capacity(
    RuneArray* self,
    usize element_size,
    intptr capacity);

void rune_runtime_array_initialize_with_elements(
    RuneArray* self,
    RunePackExpansion const* elements);

void* rune_runtime_array_remove(RuneArray* self, intptr at);

void* rune_runtime_array_first(RuneArray const* self);

void* rune_runtime_array_last(RuneArray const* self);

bool rune_runtime_array_index_of(RuneArray const* self, void const* element, usize* out_index);

void rune_runtime_hashmap_initialize(
    RuneHashMap* self,
    usize key_size,
    usize value_size,
    RuneHashFn hash_fn,
    RuneEqualFn equal_fn);

void* rune_runtime_hashmap_try_get(RuneHashMap* self, void const* key);

void rune_runtime_hashset_initialize(
    RuneHashSet* self,
    usize element_size,
    RuneHashFn hash_fn,
    RuneEqualFn equal_fn);

void rune_runtime_sortedmap_initialize(
    RuneSortedMap* self,
    usize key_size,
    usize value_size);

void* rune_runtime_sortedmap_try_get(RuneSortedMap* self, void const* key);

void rune_runtime_sortedset_initialize(RuneSortedSet* self, usize element_size);

void rune_runtime_inlinearray_to_slice(RuneSlice* out, RuneInlineArray const* self);

void rune_runtime_inlinearray_create(RuneInlineArray* out, intptr count, usize stride);

void rune_runtime_inlinearray_iterator_create(RuneContiguousIterator* out, RuneInlineArray const* self);

void rune_runtime_buffer_iterator_create(RuneContiguousIterator* out, RuneBuffer const* self, intptr count);

void rune_runtime_array_iterator_create(RuneContiguousIterator* out, RuneArray const* self);

#ifdef __cplusplus
}
#endif
