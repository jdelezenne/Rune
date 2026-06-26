#pragma once

#include <rune/types.h>

typedef struct {
    RuneChar code_point;
    uint8 bytes;
    bool valid;
} Utf8DecodeResult;

static const RuneChar utf8_replacement_character = (RuneChar)0xFFFD;
static const RuneChar utf8_max_codepoint = (RuneChar)0x10FFFF;

static inline uint8 utf8_scalar_length(uint8 first_byte)
{
    if ((first_byte & 0x80) == 0)
    {
        return 1;
    }

    if ((first_byte & 0xE0) == 0xC0)
    {
        return 2;
    }

    if ((first_byte & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((first_byte & 0xF8) == 0xF0)
    {
        return 4;
    }

    return 0;
}

static inline Utf8DecodeResult utf8_decode(uint8 const* bytes, usize length, usize offset)
{
    if (bytes == nullptr || offset >= length)
    {
        return (Utf8DecodeResult){utf8_replacement_character, 0, false};
    }

    uint8 const first = bytes[offset];
    uint8 const count = utf8_scalar_length(first);
    if (count == 0)
    {
        return (Utf8DecodeResult){utf8_replacement_character, 1, false};
    }

    if (count == 1)
    {
        return (Utf8DecodeResult){(RuneChar)(first), 1, true};
    }

    if (offset + count > length)
    {
        return (Utf8DecodeResult){utf8_replacement_character, 1, false};
    }

    RuneChar code_point = first & (count == 2 ? 0x1F : count == 3 ? 0x0F : 0x07);
    for (uint8 index = 1; index < count; ++index)
    {
        uint8 const byte = bytes[offset + index];
        if ((byte & 0xC0) != 0x80)
        {
            return (Utf8DecodeResult){utf8_replacement_character, 1, false};
        }

        code_point = (code_point << 6) | (byte & 0x3F);
    }

    if ((count == 2 && code_point < 0x80) || (count == 3 && code_point < 0x800) ||
        (count == 4 && code_point < 0x10000) || code_point > utf8_max_codepoint ||
        (code_point >= 0xD800 && code_point <= 0xDFFF))
    {
        return (Utf8DecodeResult){utf8_replacement_character, count, false};
    }

    return (Utf8DecodeResult){code_point, count, true};
}

static inline uint8 utf8_encode(RuneChar code_point, uint8* output)
{
    if (output == nullptr)
    {
        return 0;
    }

    if (code_point <= 0x7F)
    {
        output[0] = (uint8)(code_point);
        return 1;
    }

    if (code_point <= 0x7FF)
    {
        output[0] = (uint8)(0xC0 | ((code_point >> 6) & 0x1F));
        output[1] = (uint8)(0x80 | (code_point & 0x3F));
        return 2;
    }

    if (code_point <= 0xFFFF)
    {
        output[0] = (uint8)(0xE0 | ((code_point >> 12) & 0x0F));
        output[1] = (uint8)(0x80 | ((code_point >> 6) & 0x3F));
        output[2] = (uint8)(0x80 | (code_point & 0x3F));
        return 3;
    }

    if (code_point <= utf8_max_codepoint)
    {
        output[0] = (uint8)(0xF0 | ((code_point >> 18) & 0x07));
        output[1] = (uint8)(0x80 | ((code_point >> 12) & 0x3F));
        output[2] = (uint8)(0x80 | ((code_point >> 6) & 0x3F));
        output[3] = (uint8)(0x80 | (code_point & 0x3F));
        return 4;
    }

    return utf8_encode(utf8_replacement_character, output);
}

static inline intptr utf8_codepoint_count(uint8 const* bytes, usize length)
{
    intptr count = 0;
    usize offset = 0;
    while (offset < length)
    {
        Utf8DecodeResult const result = utf8_decode(bytes, length, offset);
        offset += result.valid ? result.bytes : 1;
        ++count;
    }

    return count;
}

static inline bool utf8_byte_offset_at_scalar_index(
    uint8 const* bytes,
    usize length,
    intptr scalar_index,
    usize* out_offset)
{
    if (bytes == nullptr || scalar_index < 0)
    {
        return false;
    }

    usize currentOffset = 0;
    intptr index = 0;
    while (currentOffset < length)
    {
        if (index == scalar_index)
        {
            *out_offset = currentOffset;
            return true;
        }

        Utf8DecodeResult const result = utf8_decode(bytes, length, currentOffset);
        currentOffset += result.valid ? result.bytes : 1;
        ++index;
    }

    if (index == scalar_index)
    {
        *out_offset = length;
        return true;
    }

    return false;
}

static inline bool utf8_codepoint_at_scalar_index(
    uint8 const* bytes,
    usize length,
    intptr scalar_index,
    RuneChar* out_codepoint)
{
    usize offset = 0;
    if (!utf8_byte_offset_at_scalar_index(bytes, length, scalar_index, &offset))
    {
        return false;
    }

    Utf8DecodeResult const result = utf8_decode(bytes, length, offset);
    if (!result.valid)
    {
        return false;
    }

    *out_codepoint = result.code_point;
    return true;
}
