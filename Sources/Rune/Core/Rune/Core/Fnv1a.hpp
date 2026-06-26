#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace Fnv1a
    {
        inline constexpr uint64 offsetBasis = 0xcbf29ce484222325ULL;
        inline constexpr uint64 prime = 0x100000001b3ULL;
        inline constexpr uint32 offsetBasis32 = 2166136261u;
        inline constexpr uint32 prime32 = 16777619u;

        constexpr uint64 staticHash(StringView str, uint64 value = offsetBasis)
        {
            if (str.empty())
            {
                return value;
            }

            return staticHash(str.substr(1), (value ^ static_cast<uint64>(str[0])) * prime);
        }

        inline uint64 hash(StringView str)
        {
            uint64 value = offsetBasis;
            while (!str.empty())
            {
                value ^= static_cast<uint64>(str[0]);
                value *= prime;
                str = str.substr(1);
            }

            return value;
        }

        inline uint32 hash32Bytes(Array<uint8> const& bytes)
        {
            uint32 value = offsetBasis32;
            for (uint8 byte : bytes)
            {
                value ^= byte;
                value *= prime32;
            }

            return value;
        }
    }
}
