#pragma once

#include <unordered_map>

namespace rune
{
    /// Hash map from keys to values.
    /// @tparam K Key type.
    /// @tparam V Value type.
    template <typename K, typename V>
    using HashMap = std::unordered_map<K, V>;
}
