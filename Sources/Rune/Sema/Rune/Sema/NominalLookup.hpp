#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/IdentifierTable.hpp"
#include "Rune/Sema/TypeBinding.hpp"

namespace rune
{
    struct InterfaceDeclRecord;
    struct Module;
}

namespace rune::sema
{
    TypeID resolveType(TypeArena& types, IdentifierTable& identifiers, parser::TypeExpression const& typeExpression);

    /// Returns the SemanticDecl for the nominal type (struct/enum/union/interface/alias/distinct)
    /// that owns typeId. Works for both source and imported types. Returns nullptr on miss.
    SemanticDecl const* findNominalSemanticDecl(ASTContext const& context, TypeID typeId);

    /// Resolves a Named type's distinct-type base type, if any. Handles both
    /// locally-declared and interface-origin (cross-module) distinct types without
    /// reconstructing a TypeID from the name to probe a TypeID-keyed cache/map (a
    /// name-only-interned key won't match a TypeID resolved with its nominalDeclId
    /// populated, and vice versa). Does not perform generic substitution.
    Optional<TypeID> distinctBaseTypeId(ASTContext const& context, TypeID typeId);

    /// Finds the source StructDeclaration for a TypeID. Returns nullptr for imported structs.
    parser::StructDeclaration const* findStructDeclForType(ASTContext const& context, TypeID typeId);

    /// Finds the source StructDeclaration for a TypeID. Returns nullptr for imported structs.
    parser::StructDeclaration* findStructForType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// Finds the source EnumDeclaration for a TypeID. Returns nullptr for imported enums.
    parser::EnumDeclaration const* findEnumDeclForType(ASTContext const& context, TypeID typeId);

    /// Finds the source EnumDeclaration for a TypeID. Returns nullptr for imported enums.
    parser::EnumDeclaration* findEnumForType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// Finds the source EnumDeclaration for a flags TypeID. Returns nullptr for imported flags.
    parser::EnumDeclaration const* findFlagsDeclForType(ASTContext const& context, TypeID typeId);

    /// Finds the source EnumDeclaration for a flags TypeID. Returns nullptr for imported flags.
    parser::EnumDeclaration* findFlagsForType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// Finds the source UnionDeclaration for a TypeID. Returns nullptr for imported unions.
    parser::UnionDeclaration const* findUnionDeclForType(ASTContext const& context, TypeID typeId);

    /// Finds the source UnionDeclaration for a TypeID. Returns nullptr for imported unions.
    parser::UnionDeclaration* findUnionForType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// Returns the source VariableDeclaration for a named field on a struct type.
    /// Returns nullptr for imported struct fields.
    parser::VariableDeclaration const* findStructFieldDecl(
        ASTContext const& context,
        TypeID structTypeId,
        IdentifierID fieldNameId);

    /// Resolved integer raw type of an enum or flags nominal.
    /// Returns empty when the nominal has no registered integer raw type.
    Optional<TypeID> integerRawTypeId(ASTContext const& context, TypeID nominalTypeId);

    /// Resolved integer raw type read directly off an already-resolved enum declaration.
    /// Safe for cross-module/interface-origin enums, where reconstructing a TypeID from
    /// the enum's name alone can fail to find the raw type (the reconstructed TypeID
    /// doesn't carry the nominalDeclId the original resolution already had in hand).
    Optional<TypeID> integerRawTypeIdFromDecl(
        ASTContext const& context,
        parser::EnumDeclaration const& enumDecl);

    /// True when the nominal is an enum or flags type with an integer raw representation.
    bool isIntegerBackedNominalType(ASTContext const& context, TypeID typeId);

    /// True when the nominal is an enum type, whether declared locally or imported.
    bool isEnumNominalType(
        ASTContext const& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// True when the nominal is a flags type (including types exported from imported modules).
    bool isFlagsNominalType(
        ASTContext const& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    /// Raw integer type of a flags nominal (e.g. `MessageBoxFlags` -> `UInt32`).
    Optional<TypeID> flagsRawTypeId(ASTContext const& context, TypeID flagsTypeId);

    /// Registers integer raw types for enum/flags exported from imported modules.
    void registerImportedNominalIntegerRawTypes(ASTContext& context);

    bool checkProgramTypes(ASTContext& context, parser::Program const& program);

    /// Resolve a nominal member through the semantic declaration model.
    Optional<InterfaceDeclReference> findInterfaceNominalMember(
        ASTContext const& context,
        TypeID receiverTypeId,
        StringView memberName);

    enum class NominalMemberResolutionKind : uint8
    {
        Decl,
        EnumOrFlagsCase,
        UnionCase,
    };

    struct NominalMemberResolution final
    {
        NominalMemberResolutionKind kind{NominalMemberResolutionKind::Decl};
        TypeID typeId{0};
        SemanticDeclID semanticDeclId{invalidSemanticDeclId};
        Optional<InterfaceDeclReference> interfaceReference;
        GenericSubstitutionMap receiverSubstitution;
    };

    Optional<NominalMemberResolution> resolveNominalMember(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView memberName,
        parser::ASTNode const& errorNode);

    Optional<InterfaceDeclReference> interfaceDeclReferenceForSemanticDecl(
        ASTContext const& context,
        SemanticDeclID declId);

    Optional<TypeID> lookupInterfaceNominalMemberType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView memberName,
        parser::ASTNode const& errorNode,
        GenericSubstitutionMap* receiverSubstitution = nullptr);

    Optional<TypeID> lookupInterfaceWitnessAssociatedType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView associatedTypeName,
        parser::ASTNode const& errorNode);

    bool findInterfaceStructScope(
        ASTContext const& context,
        TypeID structTypeId,
        Module const*& outModule,
        IdentifierID& outInterfaceStructNameId);

    bool interfaceDeclBelongsToType(
        ASTContext const& context,
        Module const& module,
        InterfaceDeclRecord const& interfaceDecl,
        TypeID typeId);

    bool isStructType(
        ASTContext const& context,
        TypeBindContext const& bindContext,
        TypeID typeId);

    Optional<GenericSubstitutionMap> genericSubstitutionForType(
        ASTContext& context,
        parser::Statement const& nominalDecl,
        TypeID instantiatedTypeId);

    TypeID substituteGenericType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId,
        GenericSubstitutionMap const& substitution,
        parser::ASTNode const& errorNode);

    TypeID substituteGenericType(
        ASTContext& context,
        TypeID typeId,
        GenericSubstitutionMap const& substitution);

    struct SemanticFieldLookupResult final
    {
        SemanticDeclID declId{invalidSemanticDeclId};
        TypeID typeId{0};
        parser::VariableDeclaration const* sourceFieldDecl{nullptr};
    };

    Optional<SemanticFieldLookupResult> lookupStructField(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID structTypeId,
        IdentifierID fieldNameId,
        parser::ASTNode const& errorNode);

    Optional<TypeID> lookupStructFieldType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        parser::StructDeclaration const& structDecl,
        StringView fieldName,
        Optional<GenericSubstitutionMap const*> substitution,
        parser::ASTNode const& errorNode);

    Optional<TypeID> lookupNominalMemberType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView memberName,
        parser::ASTNode const& errorNode,
        GenericSubstitutionMap* receiverSubstitution = nullptr);

    /// Returns the source FunctionDeclaration for a named method on a nominal type.
    /// Returns nullptr for imported methods and non-function members. When the
    /// name is overloaded this returns the first declaration found; callers that
    /// need argument-based overload resolution should use
    /// `lookupNominalMethodDecls` and rank the candidates.
    parser::FunctionDeclaration const* lookupNominalMethodDecl(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView memberName);

    /// Collects every source FunctionDeclaration matching `memberName` reachable
    /// on a nominal type (its own members, extension members, and inherited
    /// members), in lookup order. This is the candidate set for argument-based
    /// method overload resolution; the viability/ranking check itself is shared
    /// with free-function resolution via `checkCallArguments`.
    Array<parser::FunctionDeclaration const*> lookupNominalMethodDecls(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView memberName);

    Optional<TypeID> lookupLiteralSuffixType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        StringView suffixName,
        parser::ASTNode const& errorNode,
        parser::PropertyDeclaration const** resolvedProperty = nullptr,
        TypeID* resolvedReceiverType = nullptr,
        bool allowLiteralReceiverConversion = false);

    bool isEnumOrFlagsCase(
        ASTContext const& context,
        TypeBindContext const& bindContext,
        TypeID namedTypeId,
        StringView caseName);

    /// Returns the value type produced by `operator []` for a subscriptable type.
    Optional<TypeID> lookupSubscriptReturnType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID receiverTypeId,
        Array<TypeID> const& argumentTypes,
        parser::ASTNode const& errorNode);

    Optional<InterfaceDeclReference> lookupInterfaceNominalMemberReference(
        ASTContext const& context,
        TypeID receiverTypeId,
        StringView memberName);

    Optional<InterfaceDeclReference> lookupInterfaceWitnessMemberReference(
        ASTContext const& context,
        TypeID receiverTypeId,
        StringView memberName);

    Optional<InterfaceDeclReference> lookupInterfaceWitnessMemberReference(
        ASTContext const& context,
        TypeID receiverTypeId,
        sema::SemanticDeclID interfaceDeclId,
        StringView memberName);

    Optional<InterfaceDeclReference> lookupInterfaceRequirementMemberReference(
        ASTContext const& context,
        TypeID receiverTypeId,
        StringView memberName);

    sema::SemanticDeclID resolveInterfaceDeclId(
        ASTContext& context,
        TypeBindContext const& bindContext,
        IdentifierID interfaceNameId);

    /// Returns true if the struct or extension at typeId directly declares conformance to
    /// the named interface (or transitively through interface inheritance).
    bool typeConformsToInterface(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId,
        sema::SemanticDeclID interfaceDeclId);

    /// Returns the element type of a collection/iterable type (Array<T> -> T, Range<Bound> -> Bound).
    Optional<TypeID> collectionElementType(
        ASTContext& context,
        TypeBindContext const& bindContext,
        TypeID typeId,
        parser::ASTNode const& errorNode);

    TypeID typeIdForNominalName(
        ASTContext& context,
        IdentifierID nameId,
        Array<TypeID> const& typeArguments = {});

}
