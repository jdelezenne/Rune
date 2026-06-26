#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Assert.hpp"
#include "Rune/Core/DiagnosticEngine.hpp"
#include "Rune/Core/Enum.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Platform.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Version.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/FfiBindings.hpp"
#include "Rune/Sema/ForeignBindingTable.hpp"
#include "Rune/Sema/IdentifierTable.hpp"
#include "Rune/Sema/LiteralMetadata.hpp"
#include "Rune/Sema/PrimitiveConversion.hpp"
#include "Rune/Sema/SemanticDecl.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune::parser
{
    class Expression;
    struct ArrayLiteralExpression;
    struct EnumCase;
}

namespace rune::sema
{
    using GenericSubstitutionMap = HashMap<IdentifierID, TypeID>;
}

namespace rune
{
    class ASTContext;
}

namespace rune::sema
{
    enum class PrimitiveTypeKind : uint8;
    enum class KnownStandardTypeKind : uint8;
    enum class BuiltinMacroKind : uint8;

    void seedCompilerKnownTypes(rune::ASTContext& context);
    Optional<TypeID> lookupPrimitiveType(rune::ASTContext const& context, StringView name);
    Optional<TypeID> lookupPrimitiveType(rune::ASTContext const& context, IdentifierID nameId);
    Optional<TypeID> lookupKnownStandardType(rune::ASTContext const& context, StringView name);
    Optional<TypeID> lookupKnownStandardType(rune::ASTContext const& context, IdentifierID nameId);
    Optional<TypeID> lookupCompilerKnownType(rune::ASTContext const& context, StringView name);
    Optional<TypeID> lookupCompilerKnownType(rune::ASTContext const& context, IdentifierID nameId);
    Optional<uint32> primitiveTypeSize(PrimitiveTypeKind kind);
    Optional<uint32> primitiveTypeAlignment(PrimitiveTypeKind kind);
    void registerBuiltinMacros(rune::ASTContext& context);
    PrimitiveTypeKind primitiveTypeKind(rune::ASTContext const& context, TypeID typeId);
    void registerDistinctPrimitiveKind(rune::ASTContext& context, TypeID typeId, PrimitiveTypeKind kind);
    KnownStandardTypeKind knownStandardTypeKind(rune::ASTContext const& context, TypeID typeId);
}

namespace rune
{
    /// The module produced by one compilation.
    enum class ModuleDeclKind : uint8
    {
        Executable,
        Library,
    };

    inline StringView moduleDeclKindName(ModuleDeclKind kind)
    {
        switch (kind)
        {
            case ModuleDeclKind::Executable:
                return "executable";

            case ModuleDeclKind::Library:
                return "library";
        }

        RUNE_UNREACHABLE();
    }

    enum class ModuleBuildConfiguration
    {
        Release,
        Debug,
    };

    inline StringView moduleBuildConfigurationName(ModuleBuildConfiguration configuration)
    {
        switch (configuration)
        {
            case ModuleBuildConfiguration::Release:
                return "release";

            case ModuleBuildConfiguration::Debug:
                return "debug";
        }

        RUNE_UNREACHABLE();
    }

    inline bool moduleBuildConfigurationEmitsDebugInfo(ModuleBuildConfiguration configuration)
    {
        return configuration == ModuleBuildConfiguration::Debug;
    }

    struct ModuleDecl final
    {
        String name;
        ModuleDeclKind kind{ModuleDeclKind::Executable};
        Platform::HostPlatform targetPlatform{Platform::getHostPlatform()};
        Array<String> sourceFiles;
        String manifestPath;
        Array<String> searchPaths;
        bool enableDebugAssertions{true};

        /// Returns true for the first-party Rune standard library modules (`Rune`, `Standard`).
        /// Only these modules may use @intrinsic and @builtin.
        bool isStandardLibraryModule() const
        {
            return name == "Rune" || name == "Standard";
        }
    };

    enum class SemaTraceFlag : uint8
    {
        None = 0,
        Pipeline = 1u << 0,
        Declaration = 1u << 1,
        Import = 1u << 2,
        Lookup = 1u << 3,
        Type = 1u << 4,
        All = static_cast<uint8>(Pipeline) |
              static_cast<uint8>(Declaration) |
              static_cast<uint8>(Import) |
              static_cast<uint8>(Lookup) |
              static_cast<uint8>(Type),
    };

    RUNE_DEFINE_ENUM_BITWISE_OPERATORS(SemaTraceFlag, uint8)

    constexpr bool hasSemaTraceFlag(SemaTraceFlag flags, SemaTraceFlag flag)
    {
        return hasFlag(flags, flag);
    }

    /// Flags for InterfaceDeclRecord. Bits 0-1 encode parser::AccessModifier (0=Public, 1=Internal, 2=Private).
    enum class InterfaceDeclFlag : uint16
    {
        None = 0x00,
        AccessInternal = 1u << 0,  ///< Access modifier is Internal
        AccessPrivate = 1u << 1,   ///< Access modifier is Private
        HasFieldDefault = 1u << 2, ///< Struct field has a default value
        IsVariadic = 1u << 3,      ///< Function declaration has a variadic final parameter
        LayoutC = 1u << 4,         ///< Struct declaration uses @layout(kind: c)
        IsFlagsEnum = 1u << 5,     ///< Enum declaration uses @flags
        IsMutating = 1u << 6,      ///< Function declaration mutates its receiver
        IsBuiltin = 1u << 7,       ///< Struct declaration carries @builtin (Rune module internal)
        IsStatic = 1u << 8,        ///< Declaration is a static member of its parent scope
    };

    RUNE_DEFINE_ENUM_BITWISE_OPERATORS(InterfaceDeclFlag, uint16)

    enum class InterfaceOperatorKind : uint8
    {
        None,
        PrefixUnary,
        PostfixUnary,
        Binary,
        Subscript,
        SubscriptAssignment,
    };

    /// Interface declaration record as stored in the module binary.
    struct InterfaceDeclRecord final
    {
        DeclID id{0};
        IdentifierID nameId{0};
        DeclID parentScopeId{0};
        Array<IdentifierID> parentScopeGenericParameterNameIds;
        sema::SemanticDeclKind kind{sema::SemanticDeclKind::Variable};
        DeclContextKind contextKind{DeclContextKind::Module};
        SourceFileID sourceFileId{0};
        InterfaceDeclFlag flags{InterfaceDeclFlag::None};
        InterfaceOperatorKind operatorKind{InterfaceOperatorKind::None};
        TypeID signatureTypeId{0};
        uint8 genericArity{0};
        Array<IdentifierID> genericParameterNameIds;
        /// Parallel to `genericParameterNameIds`: `GenericParameterKind` discriminant per parameter.
        Array<uint8> genericParameterKinds;
        /// Parallel constraint type ids in the interface module's type arena (0 when unconstrained).
        Array<TypeID> genericParameterConstraintTypeIds;
        /// Parallel default argument types in the interface module's type arena (0 when no default).
        Array<TypeID> genericParameterDefaultTypeIds;
        uint8 minimumArgumentCount{0};
        Array<IdentifierID> parameterNameIds;
        TypeID distinctBaseTypeId{0};
        IdentifierID baseTypeNameId{0};
        IdentifierID cBuiltinStorageNameId{0};
    };

    inline bool interfaceDeclHasFieldDefault(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::HasFieldDefault);
    }

    inline bool interfaceDeclIsVariadic(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::IsVariadic);
    }

    inline bool interfaceDeclIsMutating(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::IsMutating);
    }

    inline bool interfaceDeclHasCLayout(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::LayoutC);
    }

    inline bool interfaceDeclIsBuiltin(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::IsBuiltin);
    }

    inline bool interfaceDeclIsStatic(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::IsStatic);
    }

    inline bool interfaceDeclIsFlagsEnum(InterfaceDeclRecord const& record)
    {
        return hasFlag(record.flags, InterfaceDeclFlag::IsFlagsEnum);
    }

    inline bool interfaceDeclIsOperator(InterfaceDeclRecord const& record)
    {
        return record.operatorKind != InterfaceOperatorKind::None;
    }

    enum class InterfaceConstantKind : uint8
    {
        Int,
        Int8,
        Int16,
        Int32,
        Int64,
        UInt,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Float32,
        Float64,
        Bool,
        Character,
        StringUtf8,
        Nil,
        TypeReference,
        FunctionReference,
        ForeignBindingReference,
    };

    struct InterfaceConstantRecord final
    {
        IdentifierID id{0};
        DeclID parentScopeId{0};
        InterfaceConstantKind kind{InterfaceConstantKind::Int64};
        uint64 data{0};
        String stringValue;
    };

    struct ModuleSectionRecord final
    {
        uint32 id{0};
        String name;
        String version;
        uint64 byteLength{0};
        uint32 checksum{0};
    };

    enum class GenericImplementationBodyKind : uint8
    {
        Function,
        PropertyGetter,
        Initializer,
    };

    struct GenericImplementationBodyRecord final
    {
        DeclID declId{0};
        GenericImplementationBodyKind kind{GenericImplementationBodyKind::Function};
        uint32 payloadOffset{0};
        uint32 payloadSize{0};
    };

    /// One physical storage field of an interface struct (includes private fields for ABI).
    struct InterfaceStructStorageFieldRecord final
    {
        IdentifierID nameId{0};
        TypeID signatureTypeId{0};
        uint32 offset{0};
        uint32 size{0};
        InterfaceDeclFlag flags{InterfaceDeclFlag::None};
    };

    /// Full in-memory layout of an interface struct as stored in the package.
    struct InterfaceStructStorageLayoutRecord final
    {
        sema::SemanticDeclID semanticDeclId{sema::invalidSemanticDeclId};
        IdentifierID structNameId{0};
        TypeID signatureTypeId{0};
        uint32 size{0};
        uint32 alignment{1};
        Array<InterfaceStructStorageFieldRecord> fields;
    };

    struct InterfaceDeclReferenceRecord final
    {
        IdentifierID moduleNameId{0};
        DeclID declId{0};
        sema::SemanticDeclID semanticDeclId{sema::invalidSemanticDeclId};
    };

    /// Packed conformance record as stored in the module binary.
    struct InterfaceConformanceRecord final
    {
        TypeID conformingTypeId{0}; ///< Semantic type id in the interface in Module::interfaceTypes
        InterfaceDeclReferenceRecord conformingDecl;
        InterfaceDeclReferenceRecord interfaceDecl;
    };

    struct InterfaceInheritanceRecord final
    {
        InterfaceDeclReferenceRecord interfaceDecl;
        InterfaceDeclReferenceRecord parentInterfaceDecl;
    };

    enum class InterfaceWitnessKind : uint8
    {
        Method,
        Property,
        AssociatedType,
    };

    struct InterfaceWitnessImportRecord final
    {
        TypeID conformingTypeId{0};
        InterfaceDeclReferenceRecord conformingDecl;
        InterfaceDeclReferenceRecord interfaceDecl;
        InterfaceDeclReferenceRecord requirementDecl;
        InterfaceDeclReferenceRecord witnessDecl;
        TypeID witnessTypeId{0};
        InterfaceWitnessKind kind{InterfaceWitnessKind::Method};
    };

    /// Dependency module decoded from a `.runemodule` interface file.
    struct Module final
    {
        String name;
        String artifactPath;
        Version formatVersion;
        ModuleDeclKind kind{ModuleDeclKind::Library};
        ModuleBuildConfiguration configuration{ModuleBuildConfiguration::Release};
        Array<String> dependencyModules;
        Array<String> dependencyVersions;
        Array<String> includeHeaders;
        Array<String> linkLibraries;
        Array<String> linkDirectories;
        Array<String> sourceFiles;
        String manifestPath;
        Array<String> identifiers;
        Array<InterfaceDeclRecord> interfaceDecls;
        Array<InterfaceConstantRecord> interfaceConstants;
        Array<InterfaceStructStorageLayoutRecord> interfaceStructStorageLayouts;
        Array<InterfaceConformanceRecord> interfaceConformances;
        Array<InterfaceWitnessImportRecord> interfaceWitnesses;
        Array<InterfaceInheritanceRecord> interfaceInheritance;
        Array<SemanticType> interfaceTypes;
        Array<ForeignBinding> foreignBindings;
        Array<GenericImplementationBodyRecord> genericImplementationBodies;
        Array<uint8> genericImplementationPayloads;
        Array<ModuleSectionRecord> sections;
        bool importedIntoScope{false};
        bool accessibleAsNamespace{false};

        StringView lookupIdentifier(IdentifierID id) const
        {
            if (id >= identifiers.size())
            {
                return {};
            }

            return identifiers[id];
        }

        InterfaceDeclRecord const* findInterfaceDecl(DeclID id) const
        {
            for (InterfaceDeclRecord const& record : interfaceDecls)
            {
                if (record.id == id)
                {
                    return &record;
                }
            }
            return nullptr;
        }

        StringView lookupParentScopeName(DeclID parentScopeId) const
        {
            InterfaceDeclRecord const* parent = findInterfaceDecl(parentScopeId);
            if (parent == nullptr)
            {
                return {};
            }
            return lookupIdentifier(parent->nameId);
        }
    };

    struct TypeConformanceRecord final
    {
        TypeID conformingTypeId{0};
        sema::SemanticDeclID conformingDeclId{sema::invalidSemanticDeclId};
        sema::SemanticDeclID interfaceDeclId{sema::invalidSemanticDeclId};
    };

    struct InterfaceWitnessRecord final
    {
        TypeID conformingTypeId{0};
        sema::SemanticDeclID conformingDeclId{sema::invalidSemanticDeclId};
        sema::SemanticDeclID interfaceDeclId{sema::invalidSemanticDeclId};
        sema::SemanticDeclID requirementDeclId{sema::invalidSemanticDeclId};
        sema::SemanticDeclID witnessDeclId{sema::invalidSemanticDeclId};
        TypeID witnessTypeId{0};
        InterfaceWitnessKind kind{InterfaceWitnessKind::Method};
    };

    struct EnumCaseValue final
    {
        TypeID nominalTypeId{0};
        TypeID rawTypeId{0};
        uint64 rawValue{0};
        parser::EnumCase const* caseDecl{nullptr};
        bool isFlags{false};
    };

    struct ForeignBindingValue final
    {
        sema::SemanticDeclID semanticDeclId{sema::invalidSemanticDeclId};
        IntrinsicId id{invalidIntrinsicId};
        String moduleName;
        /// C symbol name exactly as declared in `@runtime("...")` / `@symbol("...")`.
        /// Stored in Module.abiSymbolNames for ABI-function dispatch.
        String symbolName;
        /// Whether this is a compiler intrinsic, Rune runtime call, or foreign C symbol.
        ForeignBindingKind kind{ForeignBindingKind::Intrinsic};
    };

    struct InterfaceDeclReference final
    {
        sema::SemanticDeclID semanticDeclId{sema::invalidSemanticDeclId};
        String moduleName;
        IdentifierID nameId{0};
        DeclID parentScopeId{0};
        DeclID id{0};
    };

    InterfaceDeclReference makeInterfaceDeclReference(
        ASTContext const& context,
        Module const& module,
        InterfaceDeclRecord const& record);

    struct ResolvedIterableConformance final
    {
        TypeID iterableTypeId{0};
        TypeID collectionTypeId{0};
        TypeID iteratorTypeId{0};
        TypeID elementTypeId{0};
        TypeID nextReturnTypeId{0};
        Optional<InterfaceDeclReference> makeIteratorDecl;
        Optional<InterfaceDeclReference> nextDecl;
        ForeignBindingValue makeIterator;
        ForeignBindingValue next;
    };

    /// Long-lived state for one compilation.
    class ASTContext final
    {
    public:
        struct MacroNames final
        {
            IdentifierID nameId{0};
            IdentifierID qualifiedNameId{0};
        };

        ASTContext() = default;

        explicit ASTContext(ModuleDecl moduleDecl);

        ModuleDecl const& getModuleDecl() const
        {
            return moduleDecl;
        }

        ModuleDecl& getModuleDecl()
        {
            return moduleDecl;
        }

        Array<SourceFile> const& getSourceFiles() const
        {
            return sourceFiles;
        }

        IdentifierTable& getIdentifiers()
        {
            return identifiers;
        }

        IdentifierTable const& getIdentifiers() const
        {
            return identifiers;
        }

        DeclRegistry& getDeclRegistry()
        {
            return declRegistry;
        }

        DeclRegistry const& getDeclRegistry() const
        {
            return declRegistry;
        }

        sema::SemanticDeclTable& getSemanticDecls()
        {
            return semanticDecls;
        }

        sema::SemanticDeclTable const& getSemanticDecls() const
        {
            return semanticDecls;
        }

        DiagnosticEngine& getDiagnostics()
        {
            return diagnostics;
        }

        DiagnosticEngine const& getDiagnostics() const
        {
            return diagnostics;
        }

        SemaTraceFlag getSemaTraceFlags() const
        {
            return semaTraceFlags;
        }

        void setSemaTraceFlags(SemaTraceFlag flags)
        {
            semaTraceFlags = flags;
        }

        TypeArena& getTypes()
        {
            return types;
        }

        TypeArena const& getTypes() const
        {
            return types;
        }

        Array<Module> const& getModules() const
        {
            return modules;
        }

        Module* findModule(StringView moduleName);
        Module const* findModule(StringView moduleName) const;

        void setSourceFiles(Array<String> const& paths);
        void bindSourceFilesFromProgram(Array<String> const& sourceFilePaths);

        Module* addModule(Module module);
        void updateSemanticModuleVisibility(Module const& module);

        sema::SemanticDeclID nominalSemanticDeclIdForType(TypeID typeId) const;
        sema::SemanticDeclID topLevelInterfaceSemanticDeclId(IdentifierID nameId) const;

        Array<TypeConformanceRecord> const& getTypeConformances() const
        {
            return typeConformances;
        }

        Array<InterfaceWitnessRecord> const& getInterfaceWitnesses() const
        {
            return interfaceWitnesses;
        }

        Array<InterfaceInheritanceRecord> const& getInterfaceInheritance() const
        {
            return interfaceInheritance;
        }

        Array<ForeignBinding> const& getForeignBindings() const
        {
            return foreignBindings;
        }

        Array<ForeignBinding>& getForeignBindings()
        {
            return foreignBindings;
        }

        void setForeignBindings(Array<ForeignBinding> bindings);

        Array<String> const& getIncludeHeaders() const
        {
            return includeHeaders;
        }

        Array<String>& getIncludeHeaders()
        {
            return includeHeaders;
        }

        Array<String> const& getLinkLibraries() const
        {
            return linkLibraries;
        }

        Array<String>& getLinkLibraries()
        {
            return linkLibraries;
        }

        HashMap<TypeID, TypeID> distinctBaseTypes;

        ForeignBinding const* findForeignBinding(IntrinsicId id) const;
        ForeignBinding const* findForeignBindingByDecl(DeclID declId) const;
        ForeignBinding const* findForeignBindingBySemanticDecl(
            sema::SemanticDeclID semanticDeclId) const;
        ForeignBinding const* findForeignBindingByRegistryKey(StringView registryKey) const;

        void addInterfaceInheritance(
            sema::SemanticDeclID interfaceDeclId,
            sema::SemanticDeclID parentInterfaceDeclId);
        bool interfaceExtendsInterface(
            sema::SemanticDeclID interfaceDeclId,
            sema::SemanticDeclID parentInterfaceDeclId) const;

        void addTypeConformance(TypeID conformingTypeId, sema::SemanticDeclID interfaceDeclId);
        void addTypeConformance(
            TypeID conformingTypeId,
            sema::SemanticDeclID conformingDeclId,
            sema::SemanticDeclID interfaceDeclId);
        void addInterfaceWitness(InterfaceWitnessRecord witness);
        bool typeConformsToInterface(
            TypeID conformingTypeId,
            sema::SemanticDeclID interfaceDeclId) const;

        sema::BuiltinMacroKind findBuiltinMacroKind(IdentifierID nameId) const;

        // Sema annotation queries, populated after TypeChecker::run() completes.
        Optional<TypeID> getExpressionType(parser::Expression const& expression) const;
        Array<TypeID> distinctExpressionTypeIds() const;
        Optional<TypeID> getConvertedExpressionType(parser::Expression const& expression) const;
        Optional<sema::PrimitiveConversion> getPrimitiveConversion(
            parser::Expression const& expression) const;
        Optional<sema::LiteralMetadata> getLiteralMetadata(parser::Expression const& expression) const;
        Optional<EnumCaseValue> getEnumCaseValue(parser::Expression const& expression) const;
        Optional<ForeignBindingValue> getForeignBindingValue(parser::Expression const& expression) const;
        Optional<TypeID> getDeclType(BoundDecl const* decl) const;
        Optional<TypeID> getDeclType(sema::SemanticDeclID declId) const;
        void setDeclType(sema::SemanticDeclID declId, TypeID typeId);
        Optional<TypeID> getDeclSignatureType(BoundDecl const* decl) const;
        Optional<TypeID> getDeclSignatureType(sema::SemanticDeclID declId) const;
        void setDeclSignatureType(sema::SemanticDeclID declId, TypeID typeId);

        // Declaration reference annotations, set incrementally by TypeChecker.
        // These map source AST nodes to their resolved declarations, so downstream
        // passes (e.g. the emitter) can operate entirely on declaration pointers.
        void setVariableDeclRef(parser::VariableExpression const& expr, BoundDecl const* decl);
        BoundDecl const* getVariableDeclRef(parser::VariableExpression const& expr) const;

        void setVariableNameId(parser::VariableExpression const& expr, IdentifierID nameId);
        IdentifierID getVariableNameId(parser::VariableExpression const& expr) const;

        void setMacroNames(parser::CallExpression const& call, IdentifierID nameId, IdentifierID qualifiedNameId);
        MacroNames const* getMacroNames(parser::CallExpression const& call) const;

        void setArgumentNameId(parser::Argument const& arg, IdentifierID nameId);
        IdentifierID getArgumentNameId(parser::Argument const& arg) const;

        void setForLoopItemNameId(parser::ForStatement const& statement, IdentifierID nameId);
        IdentifierID getForLoopItemNameId(parser::ForStatement const& statement) const;

        void setResolvedIterableConformance(
            parser::ForStatement const& statement,
            ResolvedIterableConformance conformance);
        Optional<ResolvedIterableConformance> getResolvedIterableConformance(
            parser::ForStatement const& statement) const;

        void setMatchPatternCaseNameId(parser::MatchPattern const& pattern, IdentifierID nameId);
        IdentifierID getMatchPatternCaseNameId(parser::MatchPattern const& pattern) const;

        void setMatchPatternBindingNameIds(
            parser::MatchPattern const& pattern,
            Array<IdentifierID> nameIds);
        Array<IdentifierID> const* getMatchPatternBindingNameIds(
            parser::MatchPattern const& pattern) const;

        void setEnumCaseNameIds(
            parser::EnumDeclaration const& decl,
            Array<IdentifierID> nameIds);
        Array<IdentifierID> const* getEnumCaseNameIds(
            parser::EnumDeclaration const& decl) const;

        void setIntegerRawTypeId(TypeID nominalTypeId, TypeID rawTypeId);
        Optional<TypeID> getIntegerRawTypeId(TypeID nominalTypeId) const;

        void setUnionCaseNameIds(
            parser::UnionDeclaration const& decl,
            Array<IdentifierID> nameIds);
        Array<IdentifierID> const* getUnionCaseNameIds(
            parser::UnionDeclaration const& decl) const;
        Array<IdentifierID> const& ensureUnionCaseNameIds(parser::UnionDeclaration const& decl);

        void setMemberNameId(parser::Expression const& expr, IdentifierID nameId);
        IdentifierID getMemberNameId(parser::Expression const& expr) const;

        void setOptionalMemberNameId(
            parser::OptionalMemberExpression const& expr,
            IdentifierID nameId);
        IdentifierID getOptionalMemberNameId(
            parser::OptionalMemberExpression const& expr) const;

        void setMemberFieldDecl(parser::MemberExpression const& expr,
                                parser::VariableDeclaration const* field);
        parser::VariableDeclaration const* getMemberFieldDecl(
            parser::MemberExpression const& expr) const;

        void setMemberPropertyDecl(parser::MemberExpression const& expr,
                                   parser::PropertyDeclaration const* property);
        parser::PropertyDeclaration const* getMemberPropertyDecl(
            parser::MemberExpression const& expr) const;

        void setTupleElementIndex(parser::MemberExpression const& expr, uint32 index);
        Optional<uint32> getTupleElementIndex(parser::MemberExpression const& expr) const;

        void setMemberInterfaceDecl(parser::MemberExpression const& expr,
                                    InterfaceDeclReference reference);
        InterfaceDeclReference const* getMemberInterfaceDecl(
            parser::MemberExpression const& expr) const;

        void setCallInterfaceDecl(parser::CallExpression const& call,
                                  InterfaceDeclReference reference);
        InterfaceDeclReference const* getCallInterfaceDecl(
            parser::CallExpression const& call) const;

        void setIndexInterfaceDecl(parser::IndexExpression const& expr,
                                   InterfaceDeclReference reference);
        InterfaceDeclReference const* getIndexInterfaceDecl(
            parser::IndexExpression const& expr) const;

        void setAllocatorMethodInterfaceDecl(
            parser::Expression const& allocatorExpression,
            IdentifierID methodNameId,
            InterfaceDeclReference reference);
        InterfaceDeclReference const* getAllocatorMethodInterfaceDecl(
            parser::Expression const& allocatorExpression,
            IdentifierID methodNameId) const;

        void setArrayLiteralInitializerInterfaceDecl(
            parser::ArrayLiteralExpression const& literal,
            InterfaceDeclReference reference);
        InterfaceDeclReference const* getArrayLiteralInitializerInterfaceDecl(
            parser::ArrayLiteralExpression const& literal) const;
        void setDictionaryLiteralInitializerInterfaceDecl(
            parser::ArrayLiteralExpression const& literal,
            InterfaceDeclReference reference);
        InterfaceDeclReference const* getDictionaryLiteralInitializerInterfaceDecl(
            parser::ArrayLiteralExpression const& literal) const;

        void setMemberInterfaceConstant(
            parser::MemberExpression const& expr,
            InterfaceConstantRecord const* constant);
        InterfaceConstantRecord const* getMemberInterfaceConstant(
            parser::MemberExpression const& expr) const;
        void setVariableInterfaceConstant(
            parser::VariableExpression const& expr,
            InterfaceConstantRecord const* constant);
        InterfaceConstantRecord const* getVariableInterfaceConstant(
            parser::VariableExpression const& expr) const;

        void setParameterDecl(parser::Parameter const& param, BoundDecl const* decl);
        BoundDecl const* getParameterDecl(parser::Parameter const& param) const;

        void setLocalVarDecl(parser::VariableDeclaration const& decl, BoundDecl const* bound);
        BoundDecl const* getLocalVarDecl(parser::VariableDeclaration const& decl) const;

        // Instance member field accessed via a bare VariableExpression inside a method body.
        void setVariableFieldDecl(parser::VariableExpression const& expr,
                                  parser::VariableDeclaration const* field);
        parser::VariableDeclaration const* getVariableFieldDecl(
            parser::VariableExpression const& expr) const;

        // Assignment expression targets.
        void setAssignTargetDecl(parser::AssignExpression const& expr, BoundDecl const* decl);
        BoundDecl const* getAssignTargetDecl(parser::AssignExpression const& expr) const;
        sema::SemanticDeclID getAssignTargetDeclId(parser::AssignExpression const& expr) const;

        void setAssignFieldDecl(parser::AssignExpression const& expr,
                                parser::VariableDeclaration const* field);
        parser::VariableDeclaration const* getAssignFieldDecl(
            parser::AssignExpression const& expr) const;

        // Resolved call target: the FunctionDeclaration the emitter should dispatch to.
        void setCallTargetDecl(parser::CallExpression const& call,
                               parser::FunctionDeclaration const* decl);
        parser::FunctionDeclaration const* getCallTargetDecl(
            parser::CallExpression const& call) const;
        void setMakeInitializerDecl(parser::MakeExpression const& make,
                                    parser::FunctionDeclaration const* decl);
        parser::FunctionDeclaration const* getMakeInitializerDecl(
            parser::MakeExpression const& make) const;

        void setCallSignatureType(parser::CallExpression const& call, TypeID signatureTypeId);
        Optional<TypeID> getCallSignatureType(parser::CallExpression const& call) const;

        void setCallGenericSubstitution(
            parser::CallExpression const& call,
            sema::GenericSubstitutionMap substitution);
        sema::GenericSubstitutionMap const* getCallGenericSubstitution(
            parser::CallExpression const& call) const;

        void setCallGenericTypeArguments(
            parser::CallExpression const& call,
            Array<TypeID> typeArguments);
        Array<TypeID> const* getCallGenericTypeArguments(
            parser::CallExpression const& call) const;

        void setBuiltinStructInit(
            parser::StructDeclaration const& structDecl,
            UniquePtr<parser::FunctionDeclaration> init);
        parser::FunctionDeclaration const* getBuiltinStructInit(
            parser::StructDeclaration const& structDecl) const;
        bool isBuiltinStructInit(parser::FunctionDeclaration const& init) const;

        // Resolved argument order: paramToArgOrder[paramIndex] = argIndex in call.arguments,
        // or -1 if the parameter uses its default value.
        void setCallParamArgOrder(parser::CallExpression const& call, Array<int> order);
        Array<int> const* getCallParamArgOrder(parser::CallExpression const& call) const;

        // Whether a member call requires evaluating and passing an explicit receiver.
        void setCallRequiresReceiver(parser::CallExpression const& call, bool requiresReceiver);
        bool getCallRequiresReceiver(parser::CallExpression const& call) const;

        void recordFfiCallArgumentBinding(
            parser::CallExpression const& call,
            usize argumentIndex,
            FfiArgumentBinding binding);
        FfiArgumentBinding const* getFfiCallArgumentBinding(
            parser::CallExpression const& call,
            usize argumentIndex) const;

        // Returns the human-readable name for any registered declaration node.
        // Scans the DeclRegistry in O(n); intended for error messages only.
        StringView nameOf(parser::Statement const& decl) const;

        // Returns the IdentifierID for a registered declaration node (0 if not found).
        // Scans the DeclRegistry; O(n).
        IdentifierID getDeclNameId(parser::Statement const& decl) const;

        void setSemaAnnotations(
            HashMap<parser::Expression const*, TypeID> expressionTypes,
            HashMap<parser::Expression const*, TypeID> convertedExpressionTypes,
            HashMap<parser::Expression const*, sema::PrimitiveConversion> primitiveConversions,
            HashMap<parser::Expression const*, sema::LiteralMetadata> literalMetadata,
            HashMap<parser::Expression const*, EnumCaseValue> enumCaseValues,
            HashMap<parser::Expression const*, ForeignBindingValue> foreignBindingValues,
            HashMap<sema::SemanticDeclID, TypeID> declTypes,
            HashMap<sema::SemanticDeclID, TypeID> declarationSignatureTypes);

        void mergeForeignBindingValues(
            HashMap<parser::Expression const*, ForeignBindingValue> const& values);

    private:
        friend void sema::seedCompilerKnownTypes(rune::ASTContext&);
        friend Optional<TypeID> sema::lookupPrimitiveType(rune::ASTContext const&, StringView);
        friend Optional<TypeID> sema::lookupPrimitiveType(rune::ASTContext const&, IdentifierID);
        friend Optional<TypeID> sema::lookupKnownStandardType(rune::ASTContext const&, StringView);
        friend Optional<TypeID> sema::lookupKnownStandardType(rune::ASTContext const&, IdentifierID);
        friend Optional<TypeID> sema::lookupCompilerKnownType(rune::ASTContext const&, StringView);
        friend Optional<TypeID> sema::lookupCompilerKnownType(rune::ASTContext const&, IdentifierID);
        friend void sema::registerBuiltinMacros(rune::ASTContext&);
        friend sema::PrimitiveTypeKind sema::primitiveTypeKind(rune::ASTContext const&, TypeID);
        friend void sema::registerDistinctPrimitiveKind(rune::ASTContext&, TypeID, sema::PrimitiveTypeKind);
        friend sema::KnownStandardTypeKind sema::knownStandardTypeKind(rune::ASTContext const&, TypeID);

        bool areCompilerKnownTypesSeeded() const
        {
            return compilerKnownTypesSeeded;
        }

        void markCompilerKnownTypesSeeded()
        {
            compilerKnownTypesSeeded = true;
        }

        void registerPrimitiveType(IdentifierID nameId, TypeID typeId, sema::PrimitiveTypeKind kind);

        void registerPrimitiveTypeName(StringView name, TypeID typeId);

        void registerKnownStandardType(IdentifierID nameId, TypeID typeId, sema::KnownStandardTypeKind kind);

        void registerKnownStandardTypeName(StringView name, TypeID typeId);

        void registerBuiltinMacro(IdentifierID nameId, sema::BuiltinMacroKind kind);

        Optional<TypeID> findPrimitiveTypeId(IdentifierID nameId) const;

        Optional<TypeID> findPrimitiveTypeByName(StringView name) const;

        Optional<TypeID> findKnownStandardTypeId(IdentifierID nameId) const;

        Optional<TypeID> findKnownStandardTypeByName(StringView name) const;

        sema::PrimitiveTypeKind findPrimitiveTypeKind(TypeID typeId) const;

        sema::KnownStandardTypeKind findKnownStandardTypeKind(TypeID typeId) const;

    private:
        ModuleDecl moduleDecl;
        Array<SourceFile> sourceFiles;
        IdentifierTable identifiers;
        DeclRegistry declRegistry;
        sema::SemanticDeclTable semanticDecls;
        DiagnosticEngine diagnostics;
        SemaTraceFlag semaTraceFlags{SemaTraceFlag::None};
        TypeArena types;
        Array<Module> modules;
        Array<ForeignBinding> foreignBindings;
        Array<String> includeHeaders;
        Array<String> linkLibraries;
        Array<InterfaceInheritanceRecord> interfaceInheritance;
        Array<TypeConformanceRecord> typeConformances;
        Array<InterfaceWitnessRecord> interfaceWitnesses;
        HashMap<IdentifierID, TypeID> primitiveTypeIds;
        HashMap<String, TypeID> primitiveTypeNames;
        HashMap<TypeID, sema::PrimitiveTypeKind> primitiveTypeKinds;
        HashMap<IdentifierID, TypeID> knownStandardTypeIds;
        HashMap<String, TypeID> knownStandardTypeNames;
        HashMap<TypeID, sema::KnownStandardTypeKind> knownStandardTypeKinds;
        HashMap<IdentifierID, sema::BuiltinMacroKind> builtinMacroKinds;
        bool compilerKnownTypesSeeded{false};
        HashMap<parser::Expression const*, TypeID> expressionTypes;
        HashMap<parser::Expression const*, TypeID> convertedExpressionTypes;
        HashMap<parser::Expression const*, sema::PrimitiveConversion> primitiveConversions;
        HashMap<parser::Expression const*, sema::LiteralMetadata> literalMetadata;
        HashMap<parser::Expression const*, EnumCaseValue> enumCaseValues;
        HashMap<parser::Expression const*, ForeignBindingValue> foreignBindingValues;
        HashMap<sema::SemanticDeclID, TypeID> declTypes;
        HashMap<sema::SemanticDeclID, TypeID> declarationSignatureTypes;
        HashMap<parser::VariableExpression const*, BoundDecl const*> variableDeclRefs;
        HashMap<parser::VariableExpression const*, IdentifierID> variableNameIds;
        HashMap<parser::Argument const*, IdentifierID> argumentNameIds;
        HashMap<parser::ForStatement const*, IdentifierID> forLoopItemNameIds;
        HashMap<parser::ForStatement const*, ResolvedIterableConformance> resolvedIterableConformances;
        HashMap<parser::MatchPattern const*, IdentifierID> matchPatternCaseNameIds;
        HashMap<parser::MatchPattern const*, Array<IdentifierID>> matchPatternBindingNameIds;
        HashMap<parser::EnumDeclaration const*, Array<IdentifierID>> enumCaseNameIds;
        HashMap<TypeID, TypeID> integerRawTypeByNominalTypeId;
        HashMap<parser::UnionDeclaration const*, Array<IdentifierID>> unionCaseNameIds;
        HashMap<parser::Expression const*, IdentifierID> memberNameIds;
        HashMap<parser::OptionalMemberExpression const*, IdentifierID> optionalMemberNameIds;
        HashMap<parser::MemberExpression const*, parser::VariableDeclaration const*> memberFieldDecls;
        HashMap<parser::MemberExpression const*, parser::PropertyDeclaration const*> memberPropertyDecls;
        HashMap<parser::MemberExpression const*, uint32> tupleElementIndices;
        HashMap<parser::Parameter const*, BoundDecl const*> parameterDecls;
        HashMap<parser::VariableDeclaration const*, BoundDecl const*> localVarDecls;
        HashMap<parser::VariableExpression const*, parser::VariableDeclaration const*> variableFieldDecls;
        HashMap<parser::MemberExpression const*, InterfaceDeclReference> memberInterfaceDecls;
        HashMap<parser::CallExpression const*, InterfaceDeclReference> callInterfaceDecls;
        HashMap<parser::IndexExpression const*, InterfaceDeclReference> indexInterfaceDecls;
        HashMap<parser::Expression const*, HashMap<IdentifierID, InterfaceDeclReference>>
            allocatorMethodInterfaceDecls;
        HashMap<parser::ArrayLiteralExpression const*, InterfaceDeclReference>
            arrayLiteralInitializerInterfaceDecls;
        HashMap<parser::ArrayLiteralExpression const*, InterfaceDeclReference>
            dictionaryLiteralInitializerInterfaceDecls;
        HashMap<parser::MemberExpression const*, InterfaceConstantRecord const*> memberInterfaceConstants;
        HashMap<parser::VariableExpression const*, InterfaceConstantRecord const*> variableInterfaceConstants;
        HashMap<parser::AssignExpression const*, sema::SemanticDeclID> assignTargetDecls;
        HashMap<parser::AssignExpression const*, parser::VariableDeclaration const*> assignFieldDecls;
        HashMap<parser::CallExpression const*, parser::FunctionDeclaration const*> callTargetDecls;
        HashMap<parser::MakeExpression const*, parser::FunctionDeclaration const*> makeInitializerDecls;
        HashMap<parser::CallExpression const*, sema::GenericSubstitutionMap> callGenericSubstitutions;
        HashMap<parser::CallExpression const*, Array<TypeID>> callGenericTypeArguments;
        HashMap<parser::CallExpression const*, TypeID> callSignatureTypes;
        Array<UniquePtr<parser::FunctionDeclaration>> builtinStructInitsStorage;
        HashMap<parser::StructDeclaration const*, parser::FunctionDeclaration const*>
            builtinStructInits;
        HashMap<parser::CallExpression const*, Array<int>> callParamArgOrders;
        HashMap<parser::CallExpression const*, bool> callRequiresReceiver;
        HashMap<parser::CallExpression const*, MacroNames> macroNames;
        HashMap<String, FfiCallSiteBindings> ffiCallSiteBindings;
        mutable HashMap<TypeID, sema::SemanticDeclID> nominalSemanticDeclIdCache;
        mutable HashMap<IdentifierID, sema::SemanticDeclID> topLevelInterfaceDeclIdCache;
    };
}
