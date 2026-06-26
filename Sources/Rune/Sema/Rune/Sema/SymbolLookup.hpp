#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/SymbolTable.hpp"

#include <functional>

namespace rune::sema
{
    enum class QualifiedPathFailureKind
    {
        None,
        UndefinedRoot,
        HiddenRootByNestedFunction,
        MissingModuleMember,
        MissingScopeMember,
        InaccessibleScopeMember,
        UnsupportedPath,
    };

    struct QualifiedPathFailure
    {
        QualifiedPathFailureKind kind{QualifiedPathFailureKind::None};
        StringView rootName;
        StringView memberName;
        Module const* module{nullptr};
        BoundDecl const* inaccessibleDecl{nullptr};
    };

    using QualifiedPathDeclTypeProvider = std::function<Optional<TypeID>(BoundDecl const&)>;

    Array<StringView> qualifiedPathComponents(parser::MemberExpression const& memberExpression);

    /// Resolves a qualified declaration path without emitting diagnostics (LSP / IDE use).
    bool resolveQualifiedPath(
        ASTContext& context,
        SymbolTable& symbols,
        parser::MemberExpression const& memberExpression,
        DeclContext const& fromContext,
        ResolvedSymbol& result,
        QualifiedPathFailure* failure = nullptr,
        QualifiedPathDeclTypeProvider const* declTypeProvider = nullptr);
}
