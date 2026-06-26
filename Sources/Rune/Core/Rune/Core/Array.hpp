#pragma once

#include "Rune/Core/Types.hpp"

#include <vector>

namespace rune
{
    /// Dynamically sized contiguous array.
    /// @tparam T Element type.
    template <typename T>
    using Array = std::vector<T>;
}
