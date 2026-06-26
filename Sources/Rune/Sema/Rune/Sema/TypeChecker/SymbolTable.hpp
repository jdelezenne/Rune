#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/SymbolTable.hpp"

namespace rune::sema
{
    bool buildSymbolTable(
        ASTContext& context,
        parser::Program const& program,
        SymbolTable& symbols);
}
