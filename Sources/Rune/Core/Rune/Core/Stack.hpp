#pragma once

#include <stack>

namespace rune
{
    /// Last-in-first-out container adapter.
    /// @tparam T Element type.
    template <typename T>
    using Stack = std::stack<T>;
}
