#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    struct Module;
}

namespace rune::sema
{
    class SymbolTable;

    using GenericSubstitutionMap = HashMap<IdentifierID, TypeID>;

    struct TypeBindContext final
    {
        ASTContext& context;
        SymbolTable* symbols{nullptr};
        DeclContext const* fromContext{nullptr};
        Optional<SourceFileID> sourceFileIdOverride{std::nullopt};
        Optional<HashMap<IdentifierID, TypeID> const*> activeGenericSubstitution{std::nullopt};
        Optional<HashMap<IdentifierID, TypeID> const*> activeGenericConstraints{std::nullopt};
        bool validateGenericConstraints{true};
    };

    bool isTypeDeclKind(parser::ASTNodeKind kind);

    bool bindTypeName(
        TypeBindContext const& bindContext,
        StringView name,
        TypeID& result,
        parser::ASTNode const& errorNode);

    /// Like bindTypeName but never emits diagnostics.
    bool tryBindTypeName(TypeBindContext const& bindContext, StringView name, TypeID& result);

    TypeID resolveBoundType(
        TypeBindContext const& bindContext,
        parser::TypeExpression const& typeExpression,
        parser::ASTNode const& errorNode);

    /// Applies declaration-level ABI semantics to an alias target. In
    /// particular, an @symbol function alias denotes a C function pointer even
    /// before the alias declaration has been type-checked and cached.
    TypeID applyAliasCallingConvention(
        ASTContext& context,
        parser::AliasDeclaration const& alias,
        TypeID targetTypeId);

    Optional<TypeID> implicitSelfTypeInContext(
        TypeBindContext const& bindContext,
        DeclContext const& fromContext,
        parser::ASTNode const& errorNode);

    bool typesAreEqual(ASTContext const& context, TypeID left, TypeID right);

    /// Resolves a type name to the canonical TypeID used by the compiler (primitives,
    /// known standard types, then user/imported nominals via internNamed).
    TypeID resolveCanonicalNamedType(ASTContext& context, IdentifierID nameId);

    /// Maps a serialized package signature type into the current module's type arena.
    /// When `sharedImportCache` is non-null, reuse it across calls for the same module
    /// so shared subgraphs (Int, String, common function shapes) are imported once.
    TypeID importInterfaceSignatureType(
        ASTContext& context,
        Module const& module,
        TypeID signatureTypeId,
        DeclID parentScopeId = 0,
        HashMap<TypeID, TypeID>* sharedImportCache = nullptr);

    bool isOptionType(ASTContext const& context, TypeID typeId, Optional<TypeID>* wrappedType = nullptr);

    bool isHashSetType(ASTContext const& context, TypeID typeId, Optional<TypeID>* elementType = nullptr);

    bool isResultType(
        ASTContext const& context,
        TypeID typeId,
        Optional<TypeID>* successType = nullptr,
        Optional<TypeID>* errorType = nullptr);

    TypeID makeGenericNamedType(
        ASTContext& context,
        StringView baseName,
        Array<TypeID> typeArguments);

    TypeID makeOptionType(ASTContext& context, TypeID wrappedType);

    TypeID makeResultType(ASTContext& context, TypeID successType, TypeID errorType);

    /// Parses a compile-time non-negative integer from a generic value argument (literal or const name).
    Optional<int64> evaluateNonNegativeConstIntegerFromTypeArgument(
        TypeBindContext const& bindContext,
        parser::TypeExpression const& argument,
        parser::ASTNode const& errorNode);

    TypeID compileTimeIntegerTypeId(ASTContext& context, int64 value);

    /// Returns the name id when `typeId` is a bare opaque distinct (no base type).
    Optional<IdentifierID> opaqueDistinctBareNameId(ASTContext const& context, TypeID typeId);

    /// Returns the opaque distinct name id when `typeId` is used outside `*D?` / `&D?`.
    Optional<IdentifierID> opaqueDistinctUsageViolationNameId(
        ASTContext const& context,
        TypeID typeId);

    String opaqueDistinctUsageErrorMessage(
        ASTContext const& context,
        IdentifierID opaqueNameId);
}
