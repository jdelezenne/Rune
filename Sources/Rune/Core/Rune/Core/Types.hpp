#pragma once

#include <cstddef>
#include <cstdint>

namespace rune
{
    /// @name Signed Integer Types
    /// @{
    using int8 = std::int8_t;   ///< 8-bit signed integer.
    using int16 = std::int16_t; ///< 16-bit signed integer.
    using int32 = std::int32_t; ///< 32-bit signed integer.
    using int64 = std::int64_t; ///< 64-bit signed integer.
    /// @}

    /// @name Unsigned Integer Types
    /// @{
    using uint8 = std::uint8_t;   ///< 8-bit unsigned integer.
    using uint16 = std::uint16_t; ///< 16-bit unsigned integer.
    using uint32 = std::uint32_t; ///< 32-bit unsigned integer.
    using uint64 = std::uint64_t; ///< 64-bit unsigned integer.
    /// @}

    /// Raw byte type.
    using byte = std::byte;

    /// @name Size and Pointer Types
    /// @{
    using uint = std::size_t;       ///< Unsigned size type.
    using intptr = std::intptr_t;   ///< Signed pointer-sized integer.
    using uintptr = std::uintptr_t; ///< Unsigned pointer-sized integer.
    using isize = std::ptrdiff_t;   ///< Signed size difference type.
    using usize = std::size_t;      ///< Unsigned size type.
    /// @}

    /// @name Floating-Point Types
    /// @{
    using float32 = float;        ///< 32-bit floating-point type.
    using float64 = double;       ///< 64-bit floating-point type.
    using float128 = long double; ///< 128-bit floating-point type.
    /// @}
}
