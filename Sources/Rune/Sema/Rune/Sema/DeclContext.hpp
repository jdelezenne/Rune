#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/Span.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/IdentifierTable.hpp"

namespace rune::parser
{
    class Expression;
    class Statement;
}

namespace rune
{
    class DiagnosticEngine;

    using DeclID = uint32;

    /// Lexical / semantic parent of a declaration.
    enum class DeclContextKind : uint8
    {
        Module,
        SourceFile,
        StaticScope,
        NominalType,
        Extension,
        Function,
        Block,
    };

    class DeclContext;

    struct BoundDecl final
    {
        DeclID id{0};
        parser::Statement* parseNode{nullptr};
        DeclContextKind contextKind{DeclContextKind::Module};
        IdentifierID nameId{0};
        IdentifierID parentScopeNameId{0};
        SourceFileID sourceFileId{0};
        parser::AccessModifier accessModifier{parser::AccessModifier::Private};
        bool isImport{false};
        bool isWildcard{false};
        Array<IdentifierID> importSubPath;
        bool isParameter{false};
        bool isImplicitSelf{false};
        bool isGenericParameter{false};
        DeclContext* parentContext{nullptr};
    };

    /// Scope node in the DeclContext tree.
    class DeclContext final
    {
    public:
        DeclContextKind kind{DeclContextKind::Module};
        DeclContext* parent{nullptr};
        parser::Statement* parseNode{nullptr};
        SourceFileID sourceFileId{0};
        Array<DeclContext*> children;
        Array<DeclID> members;
        /// Name → member decl ids in this scope (insertion order; overloads share a key).
        HashMap<IdentifierID, Array<DeclID>> membersByName;

        explicit DeclContext(
            DeclContextKind contextKind,
            DeclContext* parentContext = nullptr,
            parser::Statement* node = nullptr,
            SourceFileID fileId = 0)
            : kind(contextKind)
            , parent(parentContext)
            , parseNode(node)
            , sourceFileId(fileId)
        {
        }
    };

    /// Owns DeclContext nodes and registered BoundDecl records.
    class DeclRegistry final
    {
    public:
        DeclContext& getModuleContext()
        {
            return *moduleContext;
        }

        DeclContext const& getModuleContext() const
        {
            return *moduleContext;
        }

        Array<BoundDecl> const& getDecls() const
        {
            return decls;
        }

        Array<BoundDecl>& getDecls()
        {
            return decls;
        }

        DeclContext& createContext(
            DeclContextKind kind,
            DeclContext& parent,
            parser::Statement* parseNode = nullptr,
            SourceFileID sourceFileId = 0);
        DeclContext& createExpressionContext(
            DeclContextKind kind,
            DeclContext& parent,
            parser::Expression* parseNode,
            SourceFileID sourceFileId = 0);

        BoundDecl& registerDecl(
            parser::Statement* parseNode,
            DeclContext& parentContext,
            StringView name,
            SourceFileID sourceFileId,
            IdentifierTable& identifiers,
            bool enforceUniqueName = true);

        BoundDecl& registerImportDecl(
            parser::Statement* parseNode,
            DeclContext& parentContext,
            StringView moduleName,
            Span<StringView const> subPath,
            bool wildcard,
            SourceFileID sourceFileId,
            IdentifierTable& identifiers);

        void setDiagnosticEngine(DiagnosticEngine* engine)
        {
            diagnostics = engine;
        }

        DeclContext* findContextForParseNode(parser::Statement* parseNode);
        DeclContext const* findContextForParseNode(parser::Statement* parseNode) const;
        DeclContext* findContextForExpression(parser::Expression const* parseNode);
        DeclContext const* findContextForExpression(parser::Expression const* parseNode) const;

        BoundDecl* findDecl(DeclID id);
        BoundDecl const* findDecl(DeclID id) const;
        /// First BoundDecl registered for this parse node (primary owner).
        BoundDecl* findDeclForParseNode(parser::Statement const* parseNode);
        BoundDecl const* findDeclForParseNode(parser::Statement const* parseNode) const;
        /// All BoundDecls that share this parse node (e.g. enum + cases, property + self).
        Array<DeclID> const& declIdsForParseNode(parser::Statement const* parseNode) const;

        /// Source function declarations registered under nameId (overloads included).
        Array<DeclID> const& functionDeclsWithName(IdentifierID nameId) const;

        /// Members of a named nominal / extension / static scope (by parentScopeNameId).
        Array<DeclID> const& scopeMembersWithName(
            IdentifierID scopeNameId,
            IdentifierID memberNameId) const;

        /// Top-level / nested `static` scope declarations registered under nameId.
        Array<DeclID> const& staticScopeDeclsWithName(IdentifierID scopeNameId) const;

        /// Reindex a decl after its parentScopeNameId is updated (e.g. extension target resolve).
        void reindexScopeMember(BoundDecl const& decl, IdentifierID previousScopeNameId);

    private:
        Array<UniquePtr<DeclContext>> storage;
        Array<BoundDecl> decls;
        HashMap<parser::Expression const*, DeclContext*> expressionContexts;
        HashMap<parser::Statement const*, DeclContext*> statementContexts;
        HashMap<parser::Statement const*, Array<DeclID>> declsByParseNode;
        HashMap<IdentifierID, Array<DeclID>> functionDeclsByName;
        HashMap<IdentifierID, Array<DeclID>> staticScopeDeclsByName;
        /// (scopeNameId << 32) | memberNameId → decl ids (overloads / extension merges).
        HashMap<uint64, Array<DeclID>> scopeMembersByScopeAndName;
        UniquePtr<DeclContext> moduleContext{std::make_unique<DeclContext>(DeclContextKind::Module)};
        DeclID nextDeclId{1};
        DiagnosticEngine* diagnostics{nullptr};

        static uint64 scopeMemberKey(IdentifierID scopeNameId, IdentifierID memberNameId)
        {
            return (static_cast<uint64>(scopeNameId) << 32) |
                   static_cast<uint64>(memberNameId);
        }
    };
}
