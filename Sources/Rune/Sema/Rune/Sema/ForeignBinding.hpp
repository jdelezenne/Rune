#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    bool bindForeignBindings(ASTContext& context, parser::Program const& program);
    bool finalizeForeignBindingAbi(ASTContext& context);
}
