#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/FfiBindings.hpp"
#include "Rune/Sema/IdentifierTable.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune
{
    class ASTContext;
    struct InterfaceConstantRecord;
    struct InterfaceDeclRecord;
    struct Module;
}

namespace rune::sema
{
    using SemanticDeclID = uint32;
    constexpr SemanticDeclID invalidSemanticDeclId = 0;

    enum class SemanticDeclOriginKind : uint8
    {
        Source,
        Interface,
        Synthesized,
    };

    enum class SemanticDeclKind : uint8
    {
        Module,
        StaticScope,
        Function,
        Initializer,
        Property,
        Variable,
        Alias,
        DistinctType,
        Struct,
        Enum,
        Union,
        Interface,
        Extension,
        TypeMember,
        Operator,
        GenericParameter,
        Constant,
        Import,
    };

    struct SemanticDeclOrigin final
    {
        SemanticDeclOriginKind kind{SemanticDeclOriginKind::Synthesized};
        parser::Statement* sourceNode{nullptr};
        String interfaceModuleName;
        Module const* interfaceModule{nullptr};
        InterfaceDeclRecord const* interfaceRecord{nullptr};
        InterfaceConstantRecord const* interfaceConstant{nullptr};
        SourceFileID sourceFileId{0};
    };

    struct SemanticDecl final
    {
        SemanticDeclID id{invalidSemanticDeclId};
        SemanticDeclKind kind{SemanticDeclKind::Variable};
        SemanticDeclOrigin origin;
        IdentifierID nameId{0};
        SemanticDeclID parentDeclId{invalidSemanticDeclId};
        DeclContextKind contextKind{DeclContextKind::Module};
        parser::AccessModifier accessModifier{parser::AccessModifier::Private};
        bool exported{false};
        bool importedIntoScope{false};
        bool accessibleAsNamespace{false};
        bool isStatic{false};
        bool isConst{false};
        bool isMutating{false};
        bool isVariadic{false};
        bool hasFieldDefault{false};
        bool hasCLayout{false};
        bool isFlagsEnum{false};
        bool isParameter{false};
        bool isImplicitSelf{false};
        bool isWildcardImport{false};
        Array<IdentifierID> importSubPath;
        TypeID signatureTypeId{0};
        TypeID distinctBaseTypeId{0};
        uint8 genericArity{0};
        Array<IdentifierID> genericParameterNameIds;
        Array<uint8> genericParameterKinds;
        Array<TypeID> genericParameterConstraintTypeIds;
        Array<TypeID> genericParameterDefaultTypeIds;
        uint8 minimumArgumentCount{0};
        Array<IdentifierID> parameterNameIds;
        uint8 operatorKind{0};
        IdentifierID cBuiltinStorageNameId{0};
        IdentifierID baseTypeNameId{0};
        DeclID boundDeclId{0};
        DeclID interfaceDeclId{0};
        String interfaceModuleName;
        Module const* interfaceModule{nullptr};
        InterfaceConstantRecord const* interfaceConstant{nullptr};
        Array<SemanticDeclID> children;
    };

    class SemanticDeclTable final
    {
    public:
        SemanticDecl const* find(SemanticDeclID id) const;
        SemanticDecl* find(SemanticDeclID id);

        SemanticDecl const* findForBoundDecl(DeclID declId) const;
        SemanticDecl* findForBoundDecl(DeclID declId);

        SemanticDecl const* findForSourceNode(parser::Statement const* node) const;
        SemanticDecl* findForSourceNode(parser::Statement const* node);

        SemanticDecl const* findSourceDeclByNameAndKind(
            IdentifierID nameId,
            SemanticDeclKind kind) const;
        SemanticDecl* findSourceDeclByNameAndKind(
            IdentifierID nameId,
            SemanticDeclKind kind);

        SemanticDecl const* findForInterfaceDecl(Module const& module, DeclID declId) const;
        SemanticDecl* findForInterfaceDecl(Module const& module, DeclID declId);

        SemanticDecl& addSourceDecl(ASTContext& context, BoundDecl const& boundDecl);
        SemanticDecl& addInterfaceDecl(
            ASTContext& context,
            Module const& module,
            InterfaceDeclRecord const& record);
        SemanticDecl& addInterfaceConstant(
            ASTContext& context,
            Module const& module,
            InterfaceConstantRecord const& record);
        SemanticDecl& addSynthesizedDecl(SemanticDecl decl);
        void rebuildInterfaceParentEdges(ASTContext& context);
        /// Wire parent/child edges for one newly loaded package without clearing
        /// existing source or previously loaded interface edges.
        /// When `firstDeclIndex` is set, only scan decls from that index onward.
        void wireInterfaceParentEdgesForModule(
            ASTContext& context,
            Module const& module,
            usize firstDeclIndex = 0);
        /// Bind interface-constant metadata for Variable/Constant decls in one
        /// package after its interface decls have been materialized.
        void rebindInterfaceConstantsForModule(
            Module const& module,
            usize firstDeclIndex = 0);
        void rebuildParentEdges(DeclRegistry const& registry, ASTContext& context);
        void rebindInterfaceModulePointers(Array<Module> const& modules);

        /// Indexes an extension by its target type's nominal identity (and name fallback).
        /// Safe to call repeatedly; duplicates are ignored.
        void indexExtensionByTarget(
            SemanticDeclID extensionId,
            TypeID targetTypeId,
            ASTContext& context);

        /// Extensions that may apply to receiverTypeId. Callers must still verify
        /// with typesAreEqual / matchExtensionType / extensionTargetMatches.
        Array<SemanticDeclID> const& extensionsForReceiverType(
            TypeID receiverTypeId,
            ASTContext& context) const;

        /// Every indexed extension. Use only when target matching is not exact
        /// (e.g. literal integer/float width conversion).
        Array<SemanticDeclID> const& allExtensions() const;

        /// Interface-origin decls in module whose module-local nameId matches.
        Array<SemanticDeclID> const& findInterfaceDeclsByName(
            Module const& module,
            IdentifierID moduleLocalNameId) const;

        /// Direct children of owner whose nameId matches (overloads preserved).
        Array<SemanticDeclID> const& childrenNamed(
            SemanticDeclID ownerDeclId,
            IdentifierID nameId) const;

        Array<SemanticDecl> const& declarations() const
        {
            return decls;
        }

        Array<SemanticDecl>& declarations()
        {
            return decls;
        }

    private:
        SemanticDeclID nextId{1};
        Array<SemanticDecl> decls;
        HashMap<SemanticDeclID, usize> declIndices;
        HashMap<DeclID, SemanticDeclID> sourceDecls;
        HashMap<parser::Statement const*, SemanticDeclID> sourceDeclsByNode;
        HashMap<IdentifierID, HashMap<uint8, SemanticDeclID>> sourceDeclsByNameAndKind;
        HashMap<String, HashMap<DeclID, SemanticDeclID>> interfaceDeclsByModule;
        /// Nominal type SemanticDeclID -> extension SemanticDeclIDs targeting that type.
        HashMap<SemanticDeclID, Array<SemanticDeclID>> extensionsByNominalTarget;
        /// Fallback when the target has no resolvable nominal decl (nameId -> extensions).
        HashMap<IdentifierID, Array<SemanticDeclID>> extensionsByTargetNameId;
        Array<SemanticDeclID> allExtensionIds;
        /// module.name -> module-local nameId -> interface SemanticDeclIDs (overloads).
        HashMap<String, HashMap<IdentifierID, Array<SemanticDeclID>>> interfaceDeclsByModuleAndName;
        /// owner SemanticDeclID -> child nameId -> child SemanticDeclIDs.
        HashMap<SemanticDeclID, HashMap<IdentifierID, Array<SemanticDeclID>>> childrenByOwnerAndName;

        SemanticDecl& append(SemanticDecl decl);
        SemanticDeclID findParentForSourceDecl(BoundDecl const& boundDecl) const;
        SemanticDeclID findParentForInterfaceDecl(
            Module const& module,
            InterfaceDeclRecord const& record) const;
        void appendUniqueExtension(Array<SemanticDeclID>& list, SemanticDeclID extensionId);
        void indexChildUnderParent(
            SemanticDeclID parentId,
            SemanticDeclID childId,
            IdentifierID localNameId);
        void rebuildChildrenByNameIndex(ASTContext& context);
        IdentifierID localNameIdForDecl(ASTContext& context, SemanticDecl const& decl) const;
        static Array<SemanticDeclID> const& emptySemanticDeclIdList();
    };

    SemanticDeclKind semanticDeclKindForSource(parser::Statement const* statement);
    SemanticDeclKind semanticDeclKindForInterface(InterfaceDeclRecord const& record);
    bool isSemanticTypeDeclKind(SemanticDeclKind kind);
    bool semanticDeclNameEquals(SemanticDecl const& decl, StringView name);
    bool semanticDeclNameIdEquals(
        ASTContext const& context,
        SemanticDecl const& decl,
        IdentifierID nameId);
    Module const* semanticDeclInterfaceModule(
        ASTContext const& context,
        SemanticDecl const& decl);
    bool semanticDeclBelongsToInterfaceModule(
        ASTContext const& context,
        SemanticDecl const& decl,
        Module const& module);
    bool semanticDeclInterfaceNameIdEquals(
        ASTContext const& context,
        SemanticDecl const& decl,
        Module const& module,
        IdentifierID moduleNameId);
    bool semanticDeclsHaveSameIdentity(
        ASTContext const& context,
        SemanticDeclID left,
        SemanticDeclID right);
}
