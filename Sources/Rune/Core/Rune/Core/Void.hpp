#pragma once

#include <compare>

namespace rune
{
    /// Represents the absence of a value.
    struct Void final
    {
        constexpr auto operator<=>(Void const&) const = default;
    };
}
