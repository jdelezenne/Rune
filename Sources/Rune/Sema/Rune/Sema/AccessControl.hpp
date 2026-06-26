#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"

namespace rune::sema
{
    parser::AccessModifier getDeclAccessModifier(parser::Statement const* parseNode);

    StringView accessModifierName(parser::AccessModifier access);

    bool isExportedAccess(parser::AccessModifier access);

    parser::AccessModifier accessModifierFromInterfaceFlags(InterfaceDeclFlag flags);

    /// Least permissive access among struct fields included in an initializer signature.
    parser::AccessModifier conservativeAccessForFields(
        Array<parser::VariableDeclaration const*> const& fields);

    parser::AccessModifier conservativeAccessForInterfaceFields(
        Array<InterfaceDeclRecord const*> const& fields);

    /// Access at a use site: private is file-scoped; cross-module use requires public.
    bool isAccessibleAtUseSite(
        parser::AccessModifier declAccess,
        SourceFileID declFileId,
        SourceFileID useFileId,
        bool crossModuleUse);

    bool isAccessibleDecl(
        BoundDecl const& boundDecl,
        DeclContext const& fromContext,
        bool crossModuleUse);

    bool shouldExportDecl(BoundDecl const& boundDecl);
}
