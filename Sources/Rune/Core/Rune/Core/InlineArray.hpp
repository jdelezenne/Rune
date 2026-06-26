#pragma once

#include "Rune/Core/Types.hpp"

#include <array>

namespace rune
{
    /// Fixed-size inline array with compile-time length.
    /// @tparam T Element type.
    /// @tparam N Number of elements.
    template <typename T, usize N>
    using InlineArray = std::array<T, N>;
}
