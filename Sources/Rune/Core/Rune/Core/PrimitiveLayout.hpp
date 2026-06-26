#pragma once

#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace PrimitiveLayout
    {
        inline constexpr usize boolSize = sizeof(bool);
        inline constexpr usize boolAlignment = alignof(bool);

        inline constexpr usize byteSize = sizeof(uint8);
        inline constexpr usize byteAlignment = alignof(uint8);

        inline constexpr usize charSize = sizeof(uint32);
        inline constexpr usize charAlignment = alignof(uint32);

        inline constexpr usize int8Size = sizeof(int8);
        inline constexpr usize int8Alignment = alignof(int8);

        inline constexpr usize int16Size = sizeof(int16);
        inline constexpr usize int16Alignment = alignof(int16);

        inline constexpr usize int32Size = sizeof(int32);
        inline constexpr usize int32Alignment = alignof(int32);

        inline constexpr usize int64Size = sizeof(int64);
        inline constexpr usize int64Alignment = alignof(int64);

        /// Rune `Int`, sized to the pointer width (`intptr`).
        inline constexpr usize intSize = sizeof(intptr);
        inline constexpr usize intAlignment = alignof(intptr);

        inline constexpr usize uint8Size = sizeof(uint8);
        inline constexpr usize uint8Alignment = alignof(uint8);

        inline constexpr usize uint16Size = sizeof(uint16);
        inline constexpr usize uint16Alignment = alignof(uint16);

        inline constexpr usize uint32Size = sizeof(uint32);
        inline constexpr usize uint32Alignment = alignof(uint32);

        inline constexpr usize uint64Size = sizeof(uint64);
        inline constexpr usize uint64Alignment = alignof(uint64);

        inline constexpr usize uintSize = sizeof(uintptr);
        inline constexpr usize uintAlignment = alignof(uintptr);

        inline constexpr usize float32Size = sizeof(float32);
        inline constexpr usize float32Alignment = alignof(float32);

        inline constexpr usize float64Size = sizeof(float64);
        inline constexpr usize float64Alignment = alignof(float64);

        inline constexpr usize pointerSize = sizeof(void*);
        inline constexpr usize pointerAlignment = alignof(void*);

        static_assert(intSize == int32Size || intSize == int64Size,
                      "Rune supports 32-bit and 64-bit architectures only");
        static_assert(uintSize == intSize, "Int and UInt must have the same width");

        inline constexpr bool machineIntIs32Bit = intSize == int32Size;
        inline constexpr bool machineIntIs64Bit = intSize == int64Size;

        inline constexpr uint16 int64BitWidth = 64;
        inline constexpr uint16 float32BitWidth = 32;
        inline constexpr uint16 float64BitWidth = 64;

        inline constexpr usize bitsToBytes(usize bitWidth)
        {
            return bitWidth / 8;
        }
    }
}
