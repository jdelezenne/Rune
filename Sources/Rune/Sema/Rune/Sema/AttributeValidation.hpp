#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    bool validateProgramAttributes(ASTContext& context, parser::Program const& program);
}
