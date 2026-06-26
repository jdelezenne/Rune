#pragma once

#include <expected>

namespace rune
{
    /// Expected value-or-error type.
    /// @tparam TValue Success value type.
    /// @tparam TError Error value type.
    template <typename TValue, typename TError>
    using Expected = std::expected<TValue, TError>;

    /// Unexpected error value wrapper for Expected.
    /// @tparam TError Error value type.
    template <typename TError>
    using Unexpected = std::unexpected<TError>;
}
