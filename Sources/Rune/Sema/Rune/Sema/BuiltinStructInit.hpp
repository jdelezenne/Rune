#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune::sema
{
    bool structHasUserInitializer(parser::StructDeclaration const& structDecl);

    void collectStructInitFields(
        ASTContext& context,
        parser::StructDeclaration const& structDecl,
        Array<parser::VariableDeclaration const*>& outFields);

    parser::FunctionDeclaration* ensureBuiltinStructInit(
        ASTContext& context,
        parser::StructDeclaration& structDecl,
        DeclContext& typeContext);

    /// Registers every builtin struct `init()` before type-checking bodies so later
    /// `BoundDecl` registrations do not reallocate the decl vector and invalidate maps.
    void preregisterAllBuiltinStructInits(ASTContext& context, parser::Program const& program);
}
