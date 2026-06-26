#pragma once

#include <variant>

namespace rune
{
    /// Type-safe discriminated union.
    /// @tparam Ts Alternative value types.
    template <typename... Ts>
    using Variant = std::variant<Ts...>;

    template <typename T, typename... Ts>
    T const* variantGetIf(Variant<Ts...> const& value)
    {
        return std::get_if<T>(&value);
    }

    template <typename T, typename... Ts>
    T* variantGetIf(Variant<Ts...>& value)
    {
        return std::get_if<T>(&value);
    }
}
