#pragma once

#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/ForeignBinding.hpp"
#include "Rune/Sema/Intrinsics.hpp"

namespace rune::sema
{
    Optional<ForeignBindingValue> foreignBindingValueForSemanticDecl(
        ASTContext const& context,
        SemanticDeclID semanticDeclId,
        StringView moduleName);

    Optional<ForeignBindingValue> foreignBindingValueForNominalMember(
        ASTContext const& context,
        TypeID receiverTypeId,
        StringView memberName,
        IntrinsicDeclarationKind kind,
        Optional<usize> parameterCount = std::nullopt,
        Span<StringView const> parameterTypeNames = {});

    Optional<ForeignBindingValue> foreignBindingValueForNominalMemberName(
        ASTContext const& context,
        StringView receiverName,
        StringView memberName,
        IntrinsicDeclarationKind kind,
        Optional<usize> parameterCount = std::nullopt,
        Span<StringView const> parameterTypeNames = {});

    bool foreignBindingMatchesModuleMember(
        ASTContext const& context,
        ForeignBindingValue const& binding,
        StringView moduleName,
        StringView receiverTypeName,
        StringView memberName,
        IntrinsicDeclarationKind kind);
}
