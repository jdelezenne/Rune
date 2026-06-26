#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Parser/Operators.hpp"
#include "Rune/RIL/Primitives.hpp"

namespace rune::ril
{
    Result<BinaryOperator> rilBinaryOperator(parser::BinaryOperator op, SourceLocation location);
    Result<UnaryOperator> rilUnaryOperator(parser::UnaryOperator op, SourceLocation location);
}
