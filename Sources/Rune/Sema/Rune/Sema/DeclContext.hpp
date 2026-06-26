#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/Span.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/IdentifierTable.hpp"

namespace rune::parser
{
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

        BoundDecl* findDecl(DeclID id);
        BoundDecl const* findDecl(DeclID id) const;

    private:
        Array<UniquePtr<DeclContext>> storage;
        Array<BoundDecl> decls;
        UniquePtr<DeclContext> moduleContext{std::make_unique<DeclContext>(DeclContextKind::Module)};
        DeclID nextDeclId{1};
        DiagnosticEngine* diagnostics{nullptr};
    };
}
