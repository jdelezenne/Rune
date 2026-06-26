#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    struct Version final
    {
        uint32 major{0};
        uint32 minor{0};
        uint32 patch{0};

        constexpr bool operator==(Version const&) const = default;
        constexpr bool operator!=(Version const&) const = default;

        String toString() const;
        static Optional<Version> parse(StringView value);
    };
}
