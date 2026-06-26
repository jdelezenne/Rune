#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

#include <format>

namespace rune::sema
{
    enum class LiteralCategory : uint8
    {
        Nil,
        Bool,
        Integer,
        FloatingPoint,
        Char,
        String,
        Array,
        Dictionary,
    };

    struct LiteralMetadata final
    {
        LiteralCategory category{LiteralCategory::Integer};
        /// Integer literals carry a full-range magnitude with an explicit sign
        /// so every value in [-(2^63), 2^64 - 1] is representable (UInt64 max
        /// and Int64 min included).
        uint64 integerMagnitude{0};
        bool integerIsNegative{false};
        float64 floatValue{0.0};
        bool booleanValue{false};
        UTF8::CodePoint characterValue{0};
        String stringValue;
    };

    /// Source-like rendering of an integer literal ("-42", "18446744073709551615").
    inline String describeIntegerLiteral(LiteralMetadata const& metadata)
    {
        return std::format(
            "{}{}",
            metadata.integerIsNegative && metadata.integerMagnitude != 0 ? "-" : "",
            metadata.integerMagnitude);
    }
}
