#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/LiteralMetadata.hpp"

namespace rune::sema
{
    String describeSemanticType(
        ASTContext const& context,
        TypeID typeId,
        Optional<LiteralMetadata> metadata);
}
