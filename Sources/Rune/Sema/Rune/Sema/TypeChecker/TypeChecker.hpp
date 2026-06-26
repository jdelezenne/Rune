#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/DiagnosticIds.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/HashSet.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/BuiltinMacros.hpp"
#include "Rune/Sema/CompilerKnownTypes.hpp"
#include "Rune/Sema/DeclSignature.hpp"
#include "Rune/Sema/LiteralTyping.hpp"
#include "Rune/Sema/NominalLookup.hpp"
#include "Rune/Sema/SymbolTable.hpp"
#include "Rune/Sema/TypeBinding.hpp"
#include "Rune/Sema/TypeConversion.hpp"

#include <algorithm>

namespace rune::sema
{
    DeclContext* findSourceFileContext(ASTContext& context, SourceFileID fileId);
    DeclContext* contextForStatementBody(
        ASTContext& context,
        parser::Statement& statement,
        DeclContext& parentContext);
    BoundDecl* findBoundDecl(ASTContext& context, parser::Statement* parseNode);
    BoundDecl* findParameterDecl(
        ASTContext& context,
        parser::FunctionDeclaration const& function,
        StringView parameterName);
    parser::StructDeclaration* findStructDeclaration(
        ASTContext& context,
        TypeBindContext& bindContext,
        TypeID typeId);
    char const* astNodeKindName(parser::ASTNodeKind kind);
    bool structRequiresInitializer(parser::StructDeclaration const& structDecl);

    class TypeChecker final
    {
    public:
        TypeChecker(ASTContext& context, parser::Program const& program, SymbolTable& symbols);

        bool run();

        bool typeCheckMonomorphizedFunctionBody(
            parser::FunctionDeclaration& function,
            GenericSubstitutionMap const& substitution);

    private:
        ASTContext& context;
        parser::Program const& program;
        SymbolTable& symbols;
        TypeBindContext bindContext;
        HashMap<parser::Expression const*, TypeID> expressionTypes;
        HashMap<parser::Expression const*, TypeID> convertedExpressionTypes;
        HashMap<parser::Expression const*, PrimitiveConversion> primitiveConversions;
        HashMap<parser::Expression const*, LiteralMetadata> literalMetadata;
        HashMap<parser::Expression const*, EnumCaseValue> enumCaseValues;
        HashMap<parser::Expression const*, ForeignBindingValue> foreignBindingValues;
        HashMap<parser::Expression const*, CallableInfo> callableInfos;
        HashMap<SemanticDeclID, TypeID> declTypes;
        HashMap<SemanticDeclID, TypeID> declarationSignatureTypes;

        struct DeferredLiteralBinding final
        {
            BoundDecl const* decl{nullptr};
            parser::VariableDeclaration* variable{nullptr};
            DeclContext const* context{nullptr};
            parser::FunctionDeclaration const* function{nullptr};
            LiteralCategory category{LiteralCategory::Integer};
            Array<parser::VariableExpression const*> uses;
        };

        HashMap<SemanticDeclID, DeferredLiteralBinding> deferredLiteralBindings;

        struct TransientBinding final
        {
            BoundDecl const* decl{nullptr};
            DeclContext const* context{nullptr};
            TypeID type{0};
        };

        HashMap<IdentifierID, Array<TransientBinding>> declTypesByName;
        parser::FunctionDeclaration const* currentFunction{nullptr};
        Optional<TypeID> currentLambdaReturnType;
        parser::PropertyDeclaration const* currentProperty{nullptr};
        bool currentPropertySetterIsMutating{false};
        uint32 tryBlockDepth{0};
        uint32 awaitExpressionDepth{0};
        uint32 constInitializerEvalDepth{0};
        HashSet<SemanticDeclID> uninitializedLocalDecls;
        HashSet<SemanticDeclID> readUninitDecls;
        enum class DeclarationCheckPhase
        {
            RegisterConformances,
            CheckBodies,
        };
        DeclarationCheckPhase declarationCheckPhase{DeclarationCheckPhase::CheckBodies};

        bool isCompletingDefiniteInitialization(BoundDecl const* decl) const;
        SemanticDeclID semanticDeclIdFor(BoundDecl const* decl) const;
        Optional<TypeID> typeForBoundDecl(BoundDecl const* decl) const;
        Optional<TypeID> signatureTypeForBoundDecl(BoundDecl const* decl) const;
        void setTypeForBoundDecl(BoundDecl const* decl, TypeID typeId);
        void setSignatureTypeForBoundDecl(BoundDecl const* decl, TypeID typeId);
        Optional<TypeID> materializeDeferredLiteralBinding(
            SemanticDeclID declId,
            TypeID targetType);
        void finalizeDeferredLiteralBindings(
            parser::FunctionDeclaration const* function);
        void markUninitialized(BoundDecl const* decl);
        void markReadBeforeInit(BoundDecl const* decl);
        bool isMarkedUninitialized(BoundDecl const* decl) const;
        void markInitialized(BoundDecl const* decl);
        static bool supportsDefiniteInitialization(DeclContext const& fromContext);
        void reportError(parser::ASTNode const& node, String message, StringView id);
        void reportWarning(parser::ASTNode const& node, String message, StringView id);
        void reportInaccessibleDecl(
            parser::ASTNode const& useNode,
            StringView name,
            BoundDecl const& decl);
        void reportUnhandledAstNode(parser::ASTNode const& node, StringView role);
        Optional<TypeID> typeOf(parser::Expression const& expression) const;
        void setExpressionType(parser::Expression const& expression, TypeID typeId);
        void setConvertedExpressionType(parser::Expression const& expression, TypeID typeId);
        void setPrimitiveConversion(
            parser::Expression const& expression,
            PrimitiveConversion conversion);
        void setEnumCaseValue(parser::Expression const& expression, EnumCaseValue value);
        void setForeignBindingValue(parser::Expression const& expression, ForeignBindingValue value);
        void setCallableInfo(parser::Expression const& expression, CallableInfo info);
        Optional<CallableInfo> callableInfoFor(parser::Expression const& expression) const;
        Optional<ForeignBindingValue> lookupForeignBindingValue(
            parser::Expression const& expression) const;
        void bindFfiCallArgument(
            parser::CallExpression const& call,
            usize parameterIndex,
            parser::Expression const& argumentExpression,
            TypeID paramTypeId,
            DeclContext const& fromContext);
        Optional<EnumCaseValue> resolveEnumCaseValue(
            TypeID nominalTypeId,
            StringView caseName,
            parser::ASTNode const& errorNode);
        bool tryRewriteEnumCaseToStringLiteral(
            DeclContext const& fromContext,
            UniquePtr<parser::Expression>& expression);
        String qualifiedNameForType(TypeID typeId) const;
        Optional<ForeignBindingValue> foreignBindingValueForInterfaceDecl(
            Module const& module,
            InterfaceDeclRecord const& interfaceDecl) const;
        Optional<ForeignBindingValue> foreignBindingValueForLocalDecl(
            BoundDecl const& localDecl) const;
        Optional<ForeignBindingValue> foreignBindingValueForCallTarget(
            InterfaceDeclRecord const* interfaceDecl,
            Module const* interfaceModule) const;
        void setDeclTypeForParseNode(parser::Statement const& statement, TypeID typeId);
        void setDeclSignatureTypeForParseNode(parser::Statement const& statement, TypeID typeId);
        void prebindExtensionSignatures();
        bool isPrefixLogicalNot(parser::UnaryExpression const& unary) const;
        bool isPostfixForceUnwrap(parser::UnaryExpression const& unary) const;
        TypeID applyTryBlockResultUnwrap(
            parser::Expression& expression,
            TypeID expressionType);
        TypeID typeIdForDecl(parser::Statement const& decl);
        Optional<TypeID> implicitSelfType(BoundDecl const& selfDecl, parser::ASTNode const& errorNode);
        Optional<TypeID> implicitSelfTypeInContext(
            DeclContext const& fromContext,
            parser::ASTNode const& errorNode);
        TypeID functionCallReturnType(
            parser::FunctionDeclaration const& function,
            parser::ASTNode const& errorNode,
            Optional<GenericSubstitutionMap const*> substitution = std::nullopt);
        bool isTupleLiteral(parser::ArrayLiteralExpression const& literal) const;
        Optional<TypeID> defaultIntegerType();
        Optional<TypeID> defaultFloatType();
        Array<StringView> qualifiedPathComponents(parser::MemberExpression const& memberExpression);
        Optional<TypeID> lookupInstanceMemberType(
            DeclContext const& fromContext,
            StringView memberName,
            parser::ASTNode const& errorNode,
            parser::VariableDeclaration const** outFieldDecl = nullptr);
        bool resolveUnqualifiedName(
            StringView name,
            DeclContext const& fromContext,
            parser::ASTNode const& node,
            bool calleeContext,
            ResolvedSymbol& result,
            bool reportErrors = true);
        TypeID typeForResolvedSymbol(ResolvedSymbol const& resolved);
        bool resolveQualifiedPath(
            parser::MemberExpression const& memberExpression,
            DeclContext const& fromContext,
            ResolvedSymbol& result,
            bool reportErrors = true);
        void annotateResolvedMemberExpression(
            parser::MemberExpression const& member,
            parser::Expression& expression,
            ResolvedSymbol const& resolved);
        void annotateMemberPathFromPackageLookup(
            parser::MemberExpression const& member,
            DeclContext const& fromContext);
        Optional<LiteralMetadata> literalMetadataFor(parser::Expression const& expression) const;
        TransientBinding const* transientBindingForName(
            IdentifierID nameId,
            DeclContext const& fromContext) const;
        void pushTransientType(
            IdentifierID nameId,
            DeclContext const& bindContext,
            TypeID typeId,
            BoundDecl const* decl = nullptr);
        void popTransientType(IdentifierID nameId, DeclContext const& bindContext);
        TypeID typeLiteralExpression(
            parser::Expression const& expression,
            LiteralMetadata metadata,
            Optional<TypeID> contextualType,
            parser::ASTNode const& errorNode);
        TypeID materializeInferredLiteral(
            parser::Expression const& expression,
            TypeID literalType,
            parser::ASTNode const& errorNode);
        TypeID checkLiteral(
            parser::LiteralExpression const& literal,
            Optional<TypeID> contextualType);
        TypeID checkDefaultExpression(
            parser::DefaultExpression const& expression,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType);
        TypeID checkUninitExpression(
            parser::UninitExpression const& expression,
            Optional<TypeID> contextualType);
        TypeID checkArrayLiteral(
            parser::ArrayLiteralExpression const& literal,
            Optional<TypeID> contextualType);
        TypeID checkExpression(
            parser::Expression& expression,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType = std::nullopt,
            bool calleeContext = false,
            bool inStatementContext = false);
        TypeID checkLambdaExpression(
            parser::LambdaExpression& lambda,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType);
        bool lambdaExpressionMatchesFunctionType(
            parser::Expression const& expression,
            TypeID functionTypeId) const;
        TypeID checkStatementExpression(
            parser::Statement& statement,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType,
            bool inStatementContext);
        TypeID checkIfExpression(
            parser::IfExpression& ifExpression,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType,
            bool inStatementContext);
        /// Type-checks an `if`/`else` chain used as a value and returns the
        /// unified result type. Shared by `checkIfExpression` (the `return if`
        /// / `let x = if` form) and by `checkStatementExpression` when a bare
        /// `if` statement is the tail of a value block or an `else if`
        /// continuation. `diagNode` locates diagnostics; the caller is
        /// responsible for `setExpressionType`/literal materialization on any
        /// enclosing expression node.
        TypeID checkIfStatementValue(
            parser::IfStatement& statement,
            parser::ASTNode const& diagNode,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType,
            bool inStatementContext);
        TypeID checkMatchExpression(
            parser::MatchExpression& matchExpression,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType,
            bool inStatementContext);
        /// Resolves an overloaded instance-method call on a nominal receiver by
        /// ranking every candidate of that name against the call arguments,
        /// using the same viability check (`checkCallArguments`) as
        /// free-function overload resolution. Returns the uniquely-viable
        /// overload; when the name is not overloaded, or no single candidate is
        /// viable, returns the first declaration in lookup order so existing
        /// single-candidate diagnostics are preserved. Returns nullptr when the
        /// receiver has no matching method.
        parser::FunctionDeclaration const* selectNominalMethodOverload(
            TypeBindContext const& bindContext,
            TypeID receiverTypeId,
            StringView memberName,
            parser::CallExpression const& call,
            DeclContext const& fromContext);
        Optional<TypeID> checkBuiltinMacroCall(
            BuiltinMacroDescriptor const& descriptor,
            parser::CallExpression& call,
            DeclContext const& fromContext,
            Optional<TypeID> contextualType,
            bool inStatementContext);
        void checkPropertyDeclaration(
            parser::PropertyDeclaration& property,
            DeclContext const& parentContext);
        TypeID functionTypeForDeclaration(
            parser::FunctionDeclaration const& function,
            parser::ASTNode const& errorNode);
        void checkFunctionDeclaration(
            parser::FunctionDeclaration& function,
            DeclContext const& parentContext);
        void refreshDeclTypePointers();
        /// When pretypedArgumentTypes is non-null, argument expressions are not
        /// re-typechecked for matching; conversion uses those types. With
        /// reportDiagnostics, literal arguments may still be rechecked against
        /// parameter contextual types for materialization.
        bool checkCallArguments(
            parser::FunctionDeclaration const& function,
            parser::CallExpression const& call,
            DeclContext const& fromContext,
            Optional<GenericSubstitutionMap const*> substitution = std::nullopt,
            bool reportDiagnostics = true,
            Array<TypeID> const* pretypedArgumentTypes = nullptr);
        /// Cheap arity/defaults/variadic gate used before silent overload probes.
        bool callArityCompatible(
            parser::FunctionDeclaration const& function,
            parser::CallExpression const& call) const;
        Array<TypeID> openCallArgumentTypes(
            parser::CallExpression const& call,
            DeclContext const& fromContext);
        bool checkCallArgumentsAgainstFunctionType(
            TypeID& functionTypeId,
            parser::CallExpression const& call,
            DeclContext const& fromContext,
            InterfaceDeclRecord const* interfaceDecl = nullptr,
            Module const* interfaceModule = nullptr,
            bool reportDiagnostics = true);
        Optional<TypeID> returnTypeForFunctionType(TypeID functionTypeId) const;
        bool checkStructInitializerCall(
            parser::StructDeclaration const& structDecl,
            parser::CallExpression const& call,
            DeclContext const& fromContext,
            Optional<TypeID> instantiatedType = std::nullopt);
        bool checkLocalStructInitializerArguments(
            parser::StructDeclaration const& structDecl,
            parser::FunctionDeclaration const& initializer,
            parser::CallExpression const& call,
            DeclContext const& fromContext,
            Optional<GenericSubstitutionMap const*> substitution);
        bool checkImportedStructInitializerCall(
            Module const& module,
            IdentifierID moduleStructNameId,
            TypeID constructedType,
            parser::CallExpression const& call,
            DeclContext const& fromContext);
        void checkVariableDeclaration(
            parser::VariableDeclaration& variable,
            DeclContext const& fromContext);
        void checkAliasDeclaration(
            parser::AliasDeclaration& alias,
            DeclContext const& fromContext);
        void checkTypeMemberDeclaration(
            parser::TypeMemberDeclaration& typeMember,
            DeclContext const& fromContext);
        void checkDistinctTypeDeclaration(
            parser::DistinctTypeDeclaration& distinctType,
            DeclContext const& fromContext);
        void checkNominalLikeDeclaration(parser::Statement& statement, DeclContext const& fromContext);
        void checkMatchStatement(parser::MatchStatement& statement, DeclContext const& fromContext);
        void checkUnionCaseArguments(
            parser::UnionDeclaration const& unionDecl,
            parser::UnionCase const& unionCase,
            TypeID instantiatedUnionType,
            parser::CallExpression const& call,
            DeclContext const& fromContext);
        void checkStatement(parser::Statement& statement, DeclContext const& fromContext);
        Optional<float64> evaluateConstNumericExpression(
            parser::Expression const& expression,
            DeclContext const& fromContext,
            HashMap<IdentifierID, float64>& knownConstants);

        /// Returns true if ancestorType appears anywhere in derivedType's inheritance chain.
        bool isAncestorType(TypeID ancestorType, TypeID derivedType) const;

        /// Returns true if a and b are related by inheritance in either direction.
        bool structTypesAreRelated(TypeID a, TypeID b) const;

        TypeID distinctTypeBaseTypeId(
            TypeID typeId,
            DeclContext const& fromContext,
            parser::CallExpression const& call);

        Optional<uint32> parseTupleElementMemberIndex(
            parser::Expression const& errorSite,
            StringView indexText,
            usize tupleElementCount);

        Optional<uint32> tupleElementIndexByName(
            SemanticType const& tupleType,
            StringView memberName) const;

        static parser::FunctionDeclaration const* asyncFunctionReference(
            ASTContext const& context,
            parser::Expression const& expression);

        bool isTaskSpawnCall(parser::CallExpression const& call) const;
        bool isTaskValueCall(parser::CallExpression const& call) const;
        Optional<TypeID> taskSpawnBodyReturnType(parser::CallExpression const& spawnCall);
        Optional<TypeID> inferTaskAwaitResultType(
            parser::CallExpression const& valueCall,
            Optional<TypeID> contextualType = std::nullopt);
    };

    bool typeCheckBodies(
        ASTContext& context,
        parser::Program const& program,
        SymbolTable& symbols);
}
