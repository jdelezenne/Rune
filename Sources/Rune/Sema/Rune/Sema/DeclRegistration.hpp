#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    /// Pass A: register parser decl nodes in DeclContext with stable DeclID.
    bool registerModuleDecls(ASTContext& context, parser::Program const& program);

    /// Registers a compiler-builtin struct init() after Pass A.
    void registerBuiltinStructMethod(
        ASTContext& context,
        parser::StructDeclaration& structDecl,
        parser::FunctionDeclaration& method);
}
