#pragma once

#include "Rune/RIL/BuilderContext.hpp"

namespace rune::ril
{
    Optional<ForeignBindingValue> importedForeignBindingValue(
        ASTContext const& context,
        ASTContext const& moduleContext,
        InterfaceDeclReference const& exported);
}
