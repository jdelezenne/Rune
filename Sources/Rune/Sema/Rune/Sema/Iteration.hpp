#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/TypeBinding.hpp"

namespace rune::parser
{
    class ASTNode;
}

namespace rune::sema
{
    Optional<ResolvedIterableConformance> resolveIterableConformance(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID iterableTypeId,
        parser::ASTNode const& errorNode);
}
