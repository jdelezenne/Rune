#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/HashSet.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    CallableInfo callableInfoForLambda(
        ASTContext const& context,
        parser::LambdaExpression const& lambda,
        HashSet<IdentifierID> const& parameterNameIds);
}
