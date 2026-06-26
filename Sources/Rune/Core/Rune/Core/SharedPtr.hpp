#pragma once
#include "Rune/Core/UniquePtr.hpp"

namespace rune
{
    /// Shared owning pointer.
    /// @tparam T Pointee type.
    template <typename T>
    using SharedPtr = std::shared_ptr<T>;
}
