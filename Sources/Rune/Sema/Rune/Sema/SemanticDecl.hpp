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

        SemanticDecl& addSourceDecl(BoundDecl const& boundDecl);
        SemanticDecl& addInterfaceDecl(Module const& module, InterfaceDeclRecord const& record);
        SemanticDecl& addInterfaceConstant(
            Module const& module,
            InterfaceConstantRecord const& record);
        SemanticDecl& addSynthesizedDecl(SemanticDecl decl);
        void rebuildParentEdges(DeclRegistry const& registry);
        void rebindInterfaceModulePointers(Array<Module> const& modules);

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

        SemanticDecl& append(SemanticDecl decl);
        SemanticDeclID findParentForSourceDecl(BoundDecl const& boundDecl) const;
        SemanticDeclID findParentForInterfaceDecl(
            Module const& module,
            InterfaceDeclRecord const& record) const;
    };

    SemanticDeclKind semanticDeclKindForSource(parser::Statement const* statement);
    SemanticDeclKind semanticDeclKindForInterface(InterfaceDeclRecord const& record);
    bool isSemanticTypeDeclKind(SemanticDeclKind kind);
    bool semanticDeclNameEquals(SemanticDecl const& decl, StringView name);
    bool semanticDeclNameIdEquals(
        ASTContext const& context,
        SemanticDecl const& decl,
        IdentifierID nameId);
    bool semanticDeclsHaveSameIdentity(
        ASTContext const& context,
        SemanticDeclID left,
        SemanticDeclID right);
}
