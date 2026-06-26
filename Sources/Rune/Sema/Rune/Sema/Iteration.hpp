#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/TypeBinding.hpp"

namespace rune::parser
{
    class ASTNode;
    enum class ForIterationMode : uint8;
}

namespace rune::sema
{
    Optional<ResolvedIterableConformance> resolveIterableConformance(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID iterableTypeId,
        parser::ForIterationMode iterationMode,
        parser::ASTNode const& errorNode);
}
