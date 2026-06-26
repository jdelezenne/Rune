#pragma once

#include "Rune/Core/Types.hpp"

#include <optional>

namespace rune
{
    /// Optional value that may or may not be present.
    /// @tparam T Value type.
    template <typename T>
    using Optional = std::optional<T>;
}
