#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::lsp
{
    Optional<SourceLocation> definitionLocationForInterfaceDecl(
        ASTContext const& context,
        Module const& module,
        InterfaceDeclRecord const& interfaceDecl);

    Optional<SourceLocation> definitionLocationForInterfaceReference(
        ASTContext const& context,
        InterfaceDeclReference const& reference);
}
