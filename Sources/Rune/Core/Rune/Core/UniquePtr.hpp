#pragma once

#include <memory>

namespace rune
{
    /// Unique owning pointer.
    /// @tparam T Pointee type.
    template <typename T>
    using UniquePtr = std::unique_ptr<T>;
}
