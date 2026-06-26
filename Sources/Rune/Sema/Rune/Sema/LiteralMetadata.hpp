#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

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
        int64 integerValue{0};
        float64 floatValue{0.0};
        bool booleanValue{false};
        UTF8::CodePoint characterValue{0};
        String stringValue;
    };
}
