#pragma once

#include "Rune/Core/Types.hpp"

namespace rune
{
    constexpr uint32 alignUp(uint32 value, uint32 alignment)
    {
        if (alignment <= 1u)
        {
            return value;
        }

        uint32 const remainder = value % alignment;
        return remainder == 0u ? value : value + (alignment - remainder);
    }

    constexpr usize alignUp(usize value, usize alignment)
    {
        if (alignment <= 1u)
        {
            return value;
        }

        usize const remainder = value % alignment;
        return remainder == 0u ? value : value + (alignment - remainder);
    }
}
