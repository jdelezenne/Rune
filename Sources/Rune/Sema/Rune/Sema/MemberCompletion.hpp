#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashSet.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"

namespace rune::sema
{
    enum class MemberCompletionKind : uint8
    {
        Field,
        Method,
        Property,
        EnumCase,
        FlagsCase,
        UnionCase,
        StaticMember,
        Constant,
        Other,
    };

    struct MemberCompletionItem final
    {
        String label;
        String detail;
        MemberCompletionKind kind{MemberCompletionKind::Other};
    };

    enum class MemberCompletionReceiverKind : uint8
    {
        Instance,
        Static,
    };

    void collectScopeMemberCompletions(
        ASTContext const& context,
        DeclContext const& fromContext,
        IdentifierID scopeNameId,
        MemberCompletionReceiverKind receiverKind,
        StringView prefix,
        HashSet<String>& seen,
        Array<MemberCompletionItem>& out);

    void collectImportedScopeMemberCompletions(
        ASTContext const& context,
        DeclContext const& fromContext,
        Module const& module,
        IdentifierID moduleScopeNameId,
        MemberCompletionReceiverKind receiverKind,
        StringView prefix,
        HashSet<String>& seen,
        Array<MemberCompletionItem>& out);

    void collectTypeMemberCompletions(
        ASTContext const& context,
        DeclContext const& fromContext,
        TypeID typeId,
        MemberCompletionReceiverKind receiverKind,
        StringView prefix,
        HashSet<String>& seen,
        Array<MemberCompletionItem>& out);

    void collectModuleMemberCompletions(
        ASTContext const& context,
        Module const& module,
        StringView prefix,
        HashSet<String>& seen,
        Array<MemberCompletionItem>& out);
}
