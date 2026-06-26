#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Parser/AST.hpp"

namespace rune::ril
{
    Result<uint64> integerLiteralValue(parser::LiteralExpression const& literal);
    Result<float64> floatLiteralValue(parser::LiteralExpression const& literal);
    Result<bool> boolLiteralValue(parser::LiteralExpression const& literal);
    Result<uint32> charLiteralValue(parser::LiteralExpression const& literal);
    Result<String> stringLiteralValue(parser::LiteralExpression const& literal);
}
