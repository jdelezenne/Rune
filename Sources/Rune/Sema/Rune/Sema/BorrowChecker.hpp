#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    bool checkBorrowRules(ASTContext& context, parser::Program const& program);
}
