#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/AccessControl.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/IdentifierTable.hpp"

namespace rune::sema
{
    struct ResolvedSymbol final
    {
        SemanticDecl const* semanticDecl{nullptr};
        BoundDecl const* sourceDecl{nullptr};
        DeclContext const* scopeContext{nullptr};
        IdentifierID scopeNameId{0};
        Optional<TypeID> ephemeralType;
    };

    BoundDecl const* sourceDeclForResolvedSymbol(
        ASTContext const& context,
        ResolvedSymbol const& resolved);
    Module const* moduleForResolvedSymbol(ASTContext const& context, ResolvedSymbol const& resolved);
    InterfaceDeclRecord const* interfaceRecordForResolvedSymbol(ResolvedSymbol const& resolved);
    InterfaceConstantRecord const* interfaceConstantForResolvedSymbol(
        ResolvedSymbol const& resolved);
    bool resolvedSymbolHasSourceOrigin(ASTContext const& context, ResolvedSymbol const& resolved);

    class SymbolTable final
    {
    public:
        void build(ASTContext& context, parser::Program const& program);

        /// Rebinds the internal context pointer after @p context is relocated (e.g. move).
        void rebindContext(ASTContext& context);

        bool lookupUnqualified(
            StringView name,
            DeclContext const& fromContext,
            ResolvedSymbol& result);

        bool lookupInaccessibleUnqualified(
            StringView name,
            DeclContext const& fromContext,
            ResolvedSymbol& result);

        /// True when `name` resolves only by crossing a nested-function lookup boundary.
        bool isHiddenByNestedFunctionBoundary(StringView name, DeclContext const& fromContext);

        /// Resolves `moduleName` in a `use` qualified path (`Module.exportedSymbol`).
        bool lookupModuleExport(
            Module const& module,
            StringView exportName,
            ResolvedSymbol& result);

        /// Resolves an exported member within an imported static scope or nominal scope.
        bool lookupImportedScopeMember(
            Module const& module,
            IdentifierID scopeNameId,
            StringView memberName,
            ResolvedSymbol& result);

        bool lookupScopeMember(
            ASTContext const& astContext,
            DeclContext const& fromContext,
            IdentifierID scopeNameId,
            StringView memberName,
            Module const* module,
            ResolvedSymbol& result,
            DeclContext const* localScopeContext = nullptr);

        bool lookupInaccessibleScopeMember(
            DeclContext const& fromContext,
            IdentifierID scopeNameId,
            StringView memberName,
            ResolvedSymbol& result);

        bool lookupQualifiedPath(
            ASTContext const& astContext,
            Array<StringView> const& components,
            DeclContext const& fromContext,
            ResolvedSymbol& result);

        static Optional<IdentifierID> findIdentifierInModule(Module const& module, StringView name);

    private:
        struct ImportEntry final
        {
            Module const* module{nullptr};
            bool wildcard{false};
            IdentifierID subScopeId{0};
        };

        ASTContext* context{nullptr};
        HashMap<SourceFileID, Array<ImportEntry>> importsByFile;

        bool findLocalMember(
            StringView name,
            DeclContext const& fromContext,
            ResolvedSymbol& result,
            DeclContext const* stopAt);

        bool findSemanticTopLevel(
            Module const& module,
            StringView name,
            ResolvedSymbol& result);

        bool findSemanticScopeMember(
            Module const& module,
            IdentifierID scopeNameId,
            StringView memberName,
            ResolvedSymbol& result);

        bool findLocalScopeMember(
            DeclContext const& fromContext,
            IdentifierID scopeNameId,
            StringView memberName,
            ResolvedSymbol& result);
    };

    /// True when the first component of a qualified member path is a static scope, type scope,
    /// module namespace, or imported package scope (not a value receiver).
    bool isNestedScopeHead(ResolvedSymbol const& head);
}
