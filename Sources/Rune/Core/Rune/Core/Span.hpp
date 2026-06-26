#pragma once

#include "Rune/Core/Types.hpp"

#include <span>

namespace rune
{
    /// Non-owning contiguous view.
    /// @tparam T Element type.
    /// @tparam Extent Static extent, or dynamic_extent.
    template <typename T, usize Extent = std::dynamic_extent>
    using Span = std::span<T, Extent>;
}
