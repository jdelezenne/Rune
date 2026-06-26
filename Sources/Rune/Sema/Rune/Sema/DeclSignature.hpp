#pragma once

#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"

namespace rune::sema
{
    TypeID extractDeclSignatureType(ASTContext& context, BoundDecl const& boundDecl);

    uint8 genericArityForDecl(parser::Statement const* parseNode);
}
