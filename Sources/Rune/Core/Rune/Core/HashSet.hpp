#pragma once

#include <unordered_set>

namespace rune
{
    /// Hash set of value elements.
    /// @tparam T Element type.
    template <typename T>
    using HashSet = std::unordered_set<T>;
}
