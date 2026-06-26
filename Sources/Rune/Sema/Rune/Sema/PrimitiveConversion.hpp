#pragma once

#include "Rune/Sema/TypeArena.hpp"

namespace rune::sema
{
    enum class PrimitiveConversionPolicy : uint8
    {
        Checked,
        OptionalChecked,
        Truncating,
        Clamping,
    };

    struct PrimitiveConversion final
    {
        TypeID sourceType{0};
        TypeID targetType{0};
        PrimitiveConversionPolicy policy{PrimitiveConversionPolicy::Checked};
    };
}
