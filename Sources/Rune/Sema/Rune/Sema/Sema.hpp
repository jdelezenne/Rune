#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    /// Pass A: register declarations and validate attributes.
    /// Pass B: bind foreign attributes and symbols.
    /// Does not bind imports or type-check bodies.
    bool prepareModule(ASTContext& context, parser::Program const& program);

    /// Pass D: build symbol table.
    /// Pass E1: resolve signatures while checking declarations.
    /// Pass F: type-check bodies and expressions.
    /// Requires Pass C import binding when the program imports modules.
    bool typeCheckProgram(ASTContext& context, parser::Program const& program);

    /// Full local-module analysis without loading imports.
    Result<bool> typeCheckModule(ASTContext& context, parser::Program const& program);
}
