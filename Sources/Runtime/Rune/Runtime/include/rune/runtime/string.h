#pragma once

#include <rune/cstring.h>
#include <rune/string.h>
#include <rune/stringslice.h>

#ifdef __cplusplus
extern "C"
{
#endif

void rune_runtime_string_create(RuneString* out);

void rune_runtime_string_from_bytes(RuneString* out, char const* bytes, usize length);

void rune_runtime_string_add(RuneString* out, RuneString const* self, RuneString const* other);

void rune_runtime_string_add_slice(RuneString* out, RuneString const* self, RuneStringSlice const* other);

bool rune_runtime_string_equal(RuneString const* self, RuneString const* other);

void rune_runtime_string_destroy(RuneString* self);

void rune_runtime_string_from_int8(RuneString* out, int8 value);

void rune_runtime_string_from_int16(RuneString* out, int16 value);

void rune_runtime_string_from_int32(RuneString* out, int32 value);

void rune_runtime_string_from_int64(RuneString* out, int64 value);

void rune_runtime_string_from_uint8(RuneString* out, uint8 value);

void rune_runtime_string_from_uint16(RuneString* out, uint16 value);

void rune_runtime_string_from_uint32(RuneString* out, uint32 value);

void rune_runtime_string_from_uint64(RuneString* out, uint64 value);

void rune_runtime_string_from_float32(RuneString* out, float32 value);

void rune_runtime_string_from_float64(RuneString* out, float64 value);

void rune_runtime_string_from_bool(RuneString* out, bool value);

void rune_runtime_string_from_stringslice(RuneString* out, RuneStringSlice const* value);

void rune_runtime_string_from_cstring(RuneString* out, RuneCString const* value);

void rune_runtime_string_from_cstring_slice(RuneString* out, RuneCStringSlice const* self);

void rune_runtime_cstring_slice_as_string_slice(RuneStringSlice* out, RuneCStringSlice const* self);

void rune_runtime_string_lower(RuneString* out, RuneString const* self);

void rune_runtime_string_upper(RuneString* out, RuneString const* self);

void rune_runtime_string_substring(RuneString* out, RuneString const* self, intptr start, intptr end);

void rune_runtime_string_insert_character_at(
    RuneString* out,
    RuneString const* self,
    RuneChar character,
    intptr at);

void rune_runtime_string_remove_character_at(RuneString* out, RuneString const* self, intptr at);

void rune_runtime_string_replace_character_at(
    RuneString* out,
    RuneString const* self,
    intptr at,
    RuneChar with);

void rune_runtime_stringslice_from_string(
    RuneStringSlice* out,
    RuneString const* self,
    intptr start,
    intptr end);

void rune_runtime_string_iterator_create(RuneStringIterator* out, RuneString const* self);

#ifdef __cplusplus
}
#endif
