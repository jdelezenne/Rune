#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/HashSet.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Declaration.hpp"
#include "Rune/RIL/Eval.hpp"
#include "Rune/RIL/Literal.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/RIL/Names.hpp"
#include "Rune/RIL/Operator.hpp"
#include "Rune/RIL/Type.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/BuiltinMacros.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/NominalLookup.hpp"
#include "Rune/Sema/TypeBinding.hpp"

#include <format>

namespace rune::ril
{
    struct ModuleBuilder;

    struct LoopTargets final
    {
        uint32 breakBlock{0};
        uint32 continueBlock{0};
    };

    struct TryCatchTarget final
    {
        TypeID resultTypeId{0};
        TypeID successTypeId{0};
        TypeID errorTypeId{0};
        Type resultType{Type::voidType()};
        Type successType{Type::voidType()};
        Type errorType{Type::voidType()};
        uint32 catchBlock{0};
        Optional<uint32> errorSlot;
    };

    struct FunctionBuilder final
    {
        struct LocalFinalizer final
        {
            uint32 slot{0};
            TypeID typeId{0};
            SourceLocation location;
        };

        ModuleBuilder& moduleBuilder;
        ASTContext const& context;
        MaterializedEvalValues const* materializedEvalValues{nullptr};
        Function function;
        HashMap<BoundDecl const*, uint32> localSlots;
        HashMap<IdentifierID, uint32> transientSlots;
        sema::GenericSubstitutionMap genericSubstitutions;
        Array<LoopTargets> loopTargets;
        Array<TryCatchTarget> tryCatchTargets;
        Array<parser::DeferStatement const*> deferredStatements;
        Array<LocalFinalizer> localFinalizers;
        Array<BoundDecl const*> lambdaCaptureDecls;
        Optional<uint32> receiverSlot;
        TypeID receiverTypeId{0};
        TypeID returnTypeId{0};
        Optional<TypeID> expectedInterfaceReferenceTypeId;
        bool constructsReceiver{false};
        bool genericBodyMode{false};
        bool lowersEvalOperand{false};
        Array<IdentifierID> genericBodyParameterNameIds;
        uint32 currentBlock{0};

        FunctionBuilder(
            ModuleBuilder& moduleBuilder,
            ASTContext const& context,
            String name,
            Type returnType,
            TypeID returnTypeId,
            Array<Type> parameters,
            bool isEntryPoint,
            TypeID receiverTypeId = 0,
            bool constructsReceiver = false,
            sema::GenericSubstitutionMap genericSubstitutions = {},
            bool genericBodyMode = false,
            Array<IdentifierID> genericBodyParameterNameIds = {},
            MaterializedEvalValues const* materializedEvalValues = nullptr,
            bool lowersEvalOperand = false);

        BasicBlock& block();

        uint32 addBlock(String name);

        Result<uint32> buildExpression(
            parser::Expression const& expression,
            Optional<Type> expectedType = std::nullopt);
        uint32 addStringLiteralValue(String value, Type type = Type::stringType());
        Result<uint32> buildMaterializedValue(
            MaterializedValue const& value,
            SourceLocation location);
        Result<uint32> buildTryUnwrappedExpression(
            parser::Expression const& expression,
            uint32 resultValue,
            Optional<Type> expectedType = std::nullopt);
        Result<uint32> buildLambdaExpression(parser::LambdaExpression const& lambda);
        Result<uint32> buildOptionalPrimitiveCastExpression(
            parser::CastExpression const& cast,
            sema::PrimitiveConversion const& conversion);
        Result<uint32> buildAddressOfExpression(parser::Expression const& expression);
        Result<Optional<uint32>> buildImportedMemberExpression(
            parser::MemberExpression const& member,
            parser::Expression const& expression,
            Optional<uint32> object = std::nullopt,
            Optional<Type> objectType = std::nullopt,
            Optional<Type> expectedType = std::nullopt);
        Result<Optional<uint32>> buildImportedAggregateMemberExpression(
            parser::MemberExpression const& member,
            parser::Expression const& expression,
            uint32 object,
            Type const& objectType,
            Optional<Type> expectedType = std::nullopt);
        Result<Optional<uint32>> buildDeclarationImplementationCallExpression(
            parser::CallExpression const& call,
            parser::Expression const& expression,
            Optional<ForeignBindingValue> binding,
            Optional<Type> expectedType = std::nullopt);
        TypeID substituteSemanticType(TypeID typeId);
        Result<Type> rilType(TypeID typeId, SourceLocation location);
        Result<Type> expressionRILType(parser::Expression const& expression);
        Optional<sema::BuiltinMacroDescriptor> builtinMacroForCall(
            parser::CallExpression const& call) const;
        Optional<uint32> aggregateFieldIndex(
            parser::VariableDeclaration const* fieldDecl,
            TypeID objectTypeId) const;
        Optional<uint32> aggregateFieldIndex(
            parser::MemberExpression const& member,
            TypeID objectTypeId) const;
        Result<uint32> buildBuiltinMacroExpression(
            parser::CallExpression const& call,
            sema::BuiltinMacroDescriptor const& macro,
            Optional<Type> expectedType = std::nullopt);
        Result<bool> buildBuiltinMacroStatement(
            parser::CallExpression const& call,
            sema::BuiltinMacroDescriptor const& macro);
        Result<TypeID> directMethodReturnTypeId(
            parser::Expression const& receiverExpression,
            StringView methodName,
            SourceLocation location);
        Result<uint32> buildDirectMethodCall(
            parser::Expression const& receiverExpression,
            StringView methodName,
            Type const& resultType,
            Array<uint32> trailingArguments,
            SourceLocation location);
        Result<uint32> buildImportedInterfaceReference(
            TypeID concreteReceiverTypeId,
            uint32 receiverAddress,
            InterfaceDeclReference const& requirementReference,
            SourceLocation location);
        Result<uint32> buildImportedInterfaceRequirementCall(
            uint32 receiverValue,
            InterfaceDeclReference const& requirementReference,
            Type const& resultType,
            Array<uint32> trailingArguments,
            SourceLocation location);
        Result<uint32> ensureRuntimeHashCallback(TypeID elementTypeId, SourceLocation location);
        Result<uint32> ensureRuntimeEqualCallback(TypeID elementTypeId, SourceLocation location);
        Result<bool> appendRuntimeCollectionCallbacks(
            Value& callValue,
            StringView symbolName,
            SourceLocation location);
        Result<bool> buildRuntimeInitialization(SourceLocation location);
        Result<uint32> buildMakeExpression(parser::MakeExpression const& make);
        Result<bool> registerLocalFinalizer(
            uint32 slot,
            TypeID typeId,
            SourceLocation location);
        Result<bool> emitFinalizerForReference(
            uint32 referenceValue,
            TypeID pointeeTypeId,
            SourceLocation location);
        Result<bool> emitLocalFinalizers(usize startIndex);
        Result<bool> buildVariableDeclaration(parser::VariableDeclaration const& declaration);
        Result<bool> buildAssignment(parser::AssignExpression const& assignment);
        Result<bool> buildReferenceAssignment(parser::BinaryExpression const& assignment);
        Result<bool> emitDeferredStatements(usize startIndex);
        Result<bool> buildBranchStatement(parser::Statement const& statement);
        Result<bool> buildIfStatement(parser::IfStatement const& statement);
        Result<bool> buildGuardStatement(parser::GuardStatement const& statement);
        Result<bool> buildWhileStatement(parser::WhileStatement const& statement);
        Result<bool> buildLoopStatement(parser::LoopStatement const& statement);
        Result<bool> buildForStatement(parser::ForStatement const& statement);
        Result<bool> buildMatchStatement(parser::MatchStatement const& statement);
        Result<bool> buildStatement(parser::Statement const& statement);
        Result<uint32> buildDefaultValue(Type const& type, SourceLocation location);
        Result<uint32> buildInitializerReceiverValue(
            Type const& type,
            TypeID typeId,
            SourceLocation location);
        Result<uint32> buildInitializedBufferValue(
            Type const& bufferType,
            Type const& elementType,
            SourceLocation location);
        Result<uint32> buildContiguousValueFromElements(
            Type const& aggregateType,
            Type const& elementType,
            Array<parser::Expression const*> const& elements,
            SourceLocation location);
        Result<uint32> buildContiguousValueFromValues(
            Type const& aggregateType,
            Type const& elementType,
            Array<uint32> const& values,
            SourceLocation location);
        Result<Function> finish(parser::FunctionDeclaration const& declaration);
        Result<Function> finishEvaluationExpression(parser::Expression const& expression);
        Result<Function> finishLambda(parser::LambdaExpression const& lambda);
        Result<Function> finishPropertyGetter(parser::PropertyDeclaration const& declaration);
        void buildForeignAbiSignatures();
    };

    struct ModuleBuilder final
    {
        ASTContext const& context;
        parser::FunctionDeclaration const* entryPoint;
        MaterializedEvalValues const* materializedEvalValues{nullptr};
        Module module;
        HashMap<BoundDecl const*, uint32> globalSlots;
        HashMap<parser::FunctionDeclaration const*, uint32> functionSlots;
        HashMap<uint32, parser::FunctionDeclaration const*> sourceFunctionSlots;
        HashMap<parser::FunctionDeclaration const*, Type> functionTypes;
        HashMap<parser::FunctionDeclaration const*, Array<Type>> functionParameterTypeMap;
        HashMap<parser::PropertyDeclaration const*, uint32> propertyGetterSlots;
        HashMap<uint32, parser::PropertyDeclaration const*> sourcePropertyGetterSlots;
        HashMap<parser::PropertyDeclaration const*, Type> propertyGetterTypes;
        HashMap<parser::PropertyDeclaration const*, Array<Type>> propertyGetterParameterTypeMap;
        HashMap<parser::FunctionDeclaration const*, uint32> functionValueAdapterSlots;
        HashMap<String, uint32> genericFunctionSlots;
        HashMap<String, uint32> importedFunctionSlots;
        HashMap<String, uint32> runtimeHashCallbackSlots;
        HashMap<String, uint32> runtimeEqualCallbackSlots;
        uint32 lambdaIndex{0};
        bool lowersEvalOperands{false};

        ModuleBuilder(ASTContext const& context, parser::FunctionDeclaration const* entryPoint);

        Result<bool> registerProgramDeclarations(parser::Program const& program);
        Result<bool> buildProgramFunctionBodies(parser::Program const& program);

        Result<uint32> ensureImportedFunction(
            parser::CallExpression const& call,
            InterfaceDeclReference const& interfaceRef,
            Type resultType);
        Result<uint32> ensureImportedFunctionForReceiver(
            InterfaceDeclReference const& interfaceRef,
            TypeID callerReceiverTypeId,
            Type resultType,
            SourceLocation location);
        Result<uint32> ensureImportedPropertyGetter(
            parser::MemberExpression const& member,
            InterfaceDeclReference const& interfaceRef,
            Type resultType);

        uint32 addReadonlyStaticData(
            String name,
            Array<uint8> bytes,
            uint32 alignment,
            Optional<Type> type = std::nullopt,
            Array<StaticData::Relocation> relocations = {})
        {
            return module.addStaticData(StaticData{
                .name = std::move(name),
                .bytes = std::move(bytes),
                .relocations = std::move(relocations),
                .alignment = alignment == 0 ? 1 : alignment,
                .type = std::move(type),
            });
        }

        uint32 addStringLiteralStaticData(StringView value)
        {
            Array<uint8> bytes;
            bytes.reserve(value.size() + 1);
            for (char const character : value)
            {
                bytes.push_back(static_cast<uint8>(character));
            }
            bytes.push_back(0);
            return addReadonlyStaticData(
                std::format("__rune_static_data_{}", module.staticData.size()),
                std::move(bytes),
                1,
                Type::unsignedInteger(8));
        }

        Result<uint32> buildModuleExpression(
            parser::Expression const& expression,
            Optional<Type> expectedType = std::nullopt)
        {
            if (auto const* literal = parser::ast_cast<parser::LiteralExpression>(&expression))
            {
                switch (literal->value.type)
                {
                    case lexer::TokenType::Integer:
                    {
                        Result<Type> type = expressionType(context, expression);
                        if (type.isError())
                        {
                            if (!expectedType.has_value())
                            {
                                return type.error();
                            }
                            type = *expectedType;
                        }

                        Result<uint64> value = integerLiteralValue(*literal);
                        if (value.isError())
                        {
                            return value.error();
                        }

                        return module.addValue(Value::integer(
                            std::move(value).unwrap(),
                            std::move(type).unwrap()));
                    }

                    case lexer::TokenType::Float:
                    {
                        Result<Type> type = expressionType(context, expression);
                        if (type.isError())
                        {
                            if (!expectedType.has_value())
                            {
                                return type.error();
                            }
                            type = *expectedType;
                        }

                        Result<float64> value = floatLiteralValue(*literal);
                        if (value.isError())
                        {
                            return value.error();
                        }

                        return module.addValue(Value::floating(
                            std::move(value).unwrap(),
                            std::move(type).unwrap()));
                    }

                    case lexer::TokenType::True:
                    case lexer::TokenType::False:
                    {
                        Result<bool> value = boolLiteralValue(*literal);
                        if (value.isError())
                        {
                            return value.error();
                        }

                        return module.addValue(Value::boolean(std::move(value).unwrap()));
                    }

                    case lexer::TokenType::Character:
                    {
                        Result<uint32> value = charLiteralValue(*literal);
                        if (value.isError())
                        {
                            return value.error();
                        }

                        return module.addValue(Value::character(std::move(value).unwrap()));
                    }
                    case lexer::TokenType::String:
                    {
                        String string = std::get<String>(literal->value.literal);
                        uint32 const data = addStringLiteralStaticData(string);
                        return module.addValue(Value::string(std::move(string), data));
                    }
                    default:
                        return SourceError("RIL builder unsupported module literal", expression.getLocation());
                }
            }

            if (auto const* variable = parser::ast_cast<parser::VariableExpression>(&expression))
            {
                BoundDecl const* decl = context.getVariableDeclRef(*variable);
                auto const slot = decl != nullptr ? globalSlots.find(decl) : globalSlots.end();
                if (decl == nullptr || slot == globalSlots.end())
                {
                    return SourceError(
                        "RIL builder only supports global references in module initializers",
                        expression.getLocation());
                }
                return module.addValue(Value::globalLoad(
                    slot->second,
                    module.globals[slot->second].type));
            }

            if (auto const* literal = parser::ast_cast<parser::ArrayLiteralExpression>(&expression))
            {
                Result<Type> type = expressionType(context, expression);
                if (type.isError())
                {
                    if (!expectedType.has_value())
                    {
                        return type.error();
                    }
                    type = *expectedType;
                }

                Type arrayType = std::move(type).unwrap();
                if (arrayType.kind != TypeKind::InlineArray)
                {
                    return SourceError(
                        "RIL builder only supports InlineArray literals in module initializers",
                        expression.getLocation());
                }

                if (literal->elements.size() != arrayType.elementCount)
                {
                    return SourceError(
                        "RIL builder inline array module initializer element count mismatch",
                        expression.getLocation());
                }

                Type const elementType = arrayElementType(arrayType);
                Array<uint32> elements;
                elements.reserve(literal->elements.size());
                for (UniquePtr<parser::Expression> const& element : literal->elements)
                {
                    if (element == nullptr)
                    {
                        return SourceError(
                            "RIL builder: malformed inline array module initializer",
                            expression.getLocation());
                    }

                    Result<uint32> value = buildModuleExpression(*element, elementType);
                    if (value.isError())
                    {
                        return value.error();
                    }

                    elements.push_back(std::move(value).unwrap());
                }

                return module.addValue(Value::makeAggregate(
                    std::move(arrayType),
                    std::move(elements)));
            }

            if (auto const* unary = parser::ast_cast<parser::UnaryExpression>(&expression))
            {
                if (!unary->isPrefix || unary->operand == nullptr)
                {
                    return SourceError("RIL builder only supports prefix unary expressions", expression.getLocation());
                }

                Result<UnaryOperator> op = rilUnaryOperator(unary->op, expression.getLocation());
                if (op.isError())
                {
                    return op.error();
                }

                Result<uint32> operand = buildModuleExpression(*unary->operand, expectedType);
                if (operand.isError())
                {
                    return operand.error();
                }

                Result<Type> type = expressionType(context, expression);
                if (type.isError())
                {
                    if (expectedType.has_value())
                    {
                        type = *expectedType;
                    }
                    else
                    {
                        type = expressionType(context, *unary->operand);
                    }
                }

                return module.addValue(Value::unary(
                    std::move(op).unwrap(),
                    std::move(type).unwrap(),
                    std::move(operand).unwrap()));
            }

            if (auto const* binary = parser::ast_cast<parser::BinaryExpression>(&expression))
            {
                if (binary->left == nullptr || binary->right == nullptr)
                {
                    return SourceError("RIL builder: malformed binary expression", expression.getLocation());
                }
                Result<BinaryOperator> op = rilBinaryOperator(binary->op, expression.getLocation());
                if (op.isError())
                {
                    return op.error();
                }

                Result<Type> type = expressionType(context, expression);
                if (type.isError())
                {
                    if (!expectedType.has_value())
                    {
                        return type.error();
                    }
                    type = *expectedType;
                }
                Type resultType = std::move(type).unwrap();

                Optional<Type> leftExpected;
                switch (op.value())
                {
                    case BinaryOperator::Add:
                    case BinaryOperator::Subtract:
                    case BinaryOperator::Multiply:
                    case BinaryOperator::Divide:
                    case BinaryOperator::Modulo:
                    case BinaryOperator::BitwiseAnd:
                    case BinaryOperator::BitwiseOr:
                    case BinaryOperator::BitwiseXor:
                    case BinaryOperator::LeftShift:
                    case BinaryOperator::RightShift:
                        leftExpected = resultType;
                        break;

                    case BinaryOperator::Equal:
                    case BinaryOperator::NotEqual:
                    case BinaryOperator::LessThan:
                    case BinaryOperator::LessThanEqual:
                    case BinaryOperator::GreaterThan:
                    case BinaryOperator::GreaterThanEqual:
                    case BinaryOperator::LogicalAnd:
                    case BinaryOperator::LogicalOr:
                        break;
                }

                Result<uint32> left = buildModuleExpression(*binary->left, leftExpected);
                if (left.isError())
                {
                    return left.error();
                }

                Type const operandType = module.values[left.value()].type;
                Result<uint32> right = buildModuleExpression(*binary->right, operandType);
                if (right.isError())
                {
                    return right.error();
                }

                return module.addValue(Value::binary(
                    std::move(op).unwrap(),
                    resultType,
                    std::move(left).unwrap(),
                    std::move(right).unwrap()));
            }

            return SourceError("RIL builder unsupported module initializer", expression.getLocation());
        }

        Result<bool> registerGlobal(parser::VariableDeclaration const& declaration)
        {
            if (declaration.isTupleDestructuring() || declaration.initializer == nullptr)
            {
                return SourceError("RIL builder requires simple initialized globals", declaration.getLocation());
            }

            BoundDecl const* decl = findRegisteredDecl(context, declaration);
            if (decl == nullptr)
            {
                return SourceError("RIL builder requires global declaration metadata", declaration.getLocation());
            }

            Optional<TypeID> const typeId = context.getDeclType(decl);
            if (!typeId.has_value())
            {
                return SourceError("RIL builder requires global type metadata", declaration.getLocation());
            }

            Result<Type> type = rilTypeFromSemanticType(context, *typeId, declaration.getLocation());
            if (type.isError())
            {
                return type.error();
            }
            Type globalType = std::move(type).unwrap();

            Result<uint32> initializer = buildModuleExpression(*declaration.initializer, globalType);
            if (initializer.isError())
            {
                return initializer.error();
            }

            uint32 const slot = module.addGlobal(Global{
                .name = declaration.name.lexeme,
                .type = globalType,
                .initializerValue = std::move(initializer).unwrap(),
                .isMutable = declaration.variableKind == parser::VariableKind::Var,
            });
            globalSlots[decl] = slot;
            return true;
        }

        TypeID receiverTypeIdForDecl(BoundDecl const* decl) const
        {
            if (decl == nullptr)
            {
                return 0;
            }

            if (decl->contextKind == DeclContextKind::NominalType &&
                (decl->parentScopeNameId != 0 ||
                 (decl->parentContext != nullptr &&
                  decl->parentContext->parseNode != nullptr)))
            {
                IdentifierID parentScopeNameId = decl->parentScopeNameId;
                bool hasParentScopeName = decl->parentScopeNameId != 0;
                ASTContext& mutableContext = const_cast<ASTContext&>(context);
                if (!hasParentScopeName)
                {
                    parser::Statement const* parentNode = decl->parentContext->parseNode;
                    switch (parentNode->getKind())
                    {
                        case parser::ASTNodeKind::StructDeclaration:
                            parentScopeNameId =
                                mutableContext.getIdentifiers().getOrInsert(
                                    static_cast<parser::StructDeclaration const&>(*parentNode)
                                        .name.lexeme);
                            hasParentScopeName = true;
                            break;
                        case parser::ASTNodeKind::UnionDeclaration:
                            parentScopeNameId =
                                mutableContext.getIdentifiers().getOrInsert(
                                    static_cast<parser::UnionDeclaration const&>(*parentNode)
                                        .name.lexeme);
                            hasParentScopeName = true;
                            break;
                        case parser::ASTNodeKind::InterfaceDeclaration:
                            parentScopeNameId =
                                mutableContext.getIdentifiers().getOrInsert(
                                    static_cast<parser::InterfaceDeclaration const&>(*parentNode)
                                        .name.lexeme);
                            hasParentScopeName = true;
                            break;
                        default:
                            break;
                    }
                }

                if (!hasParentScopeName)
                {
                    return 0;
                }

                Array<TypeID> arguments;
                if (decl->parentContext != nullptr)
                {
                    for (DeclID const memberId : decl->parentContext->members)
                    {
                        BoundDecl const* memberDecl =
                            context.getDeclRegistry().findDecl(memberId);
                        if (memberDecl == nullptr || !memberDecl->isGenericParameter)
                        {
                            continue;
                        }

                        arguments.push_back(sema::resolveCanonicalNamedType(
                            mutableContext,
                            memberDecl->nameId));
                    }
                }

                if (!arguments.empty())
                {
                    return const_cast<ASTContext&>(context).getTypes().internGeneric(
                        parentScopeNameId,
                        std::move(arguments));
                }

                return sema::resolveCanonicalNamedType(
                    mutableContext,
                    parentScopeNameId);
            }

            if (decl->contextKind == DeclContextKind::Extension &&
                decl->parentScopeNameId != 0 &&
                decl->parentContext != nullptr)
            {
                Array<TypeID> arguments;
                for (DeclID const memberId : decl->parentContext->members)
                {
                    BoundDecl const* memberDecl =
                        context.getDeclRegistry().findDecl(memberId);
                    if (memberDecl == nullptr || !memberDecl->isGenericParameter)
                    {
                        continue;
                    }

                    arguments.push_back(sema::resolveCanonicalNamedType(
                        const_cast<ASTContext&>(context),
                        memberDecl->nameId));
                }

                if (!arguments.empty())
                {
                    return const_cast<ASTContext&>(context).getTypes().internGeneric(
                        decl->parentScopeNameId,
                        std::move(arguments));
                }

                return sema::resolveCanonicalNamedType(
                    const_cast<ASTContext&>(context),
                    decl->parentScopeNameId);
            }

            if (decl->contextKind == DeclContextKind::Extension &&
                decl->parentContext != nullptr &&
                decl->parentContext->parseNode != nullptr &&
                decl->parentContext->parseNode->getKind() == parser::ASTNodeKind::ExtensionDeclaration)
            {
                return receiverTypeIdForExtension(
                    static_cast<parser::ExtensionDeclaration const&>(
                        *decl->parentContext->parseNode));
            }

            return 0;
        }

        TypeID receiverTypeIdForFunction(parser::FunctionDeclaration const& function) const
        {
            if (function.isStatic)
            {
                return 0;
            }

            return receiverTypeIdForDecl(findRegisteredDecl(context, function));
        }

        TypeID receiverTypeIdForProperty(parser::PropertyDeclaration const& property) const
        {
            return receiverTypeIdForDecl(findRegisteredDecl(context, property));
        }

        TypeID receiverTypeIdForExtension(parser::ExtensionDeclaration const& extension) const
        {
            if (extension.targetType == nullptr)
            {
                return 0;
            }

            BoundDecl const* extensionDecl = findRegisteredDecl(context, extension);
            if (extensionDecl != nullptr)
            {
                if (Optional<TypeID> const targetTypeId = context.getDeclType(extensionDecl))
                {
                    return *targetTypeId;
                }
            }

            ASTContext& mutableContext = const_cast<ASTContext&>(context);
            DeclContext const* extensionContext =
                context.getDeclRegistry().findContextForParseNode(
                    const_cast<parser::ExtensionDeclaration*>(&extension));
            sema::TypeBindContext bindContext{
                .context = mutableContext,
                .fromContext = extensionContext != nullptr
                                 ? extensionContext
                                 : &mutableContext.getDeclRegistry().getModuleContext(),
            };
            return sema::resolveBoundType(
                bindContext,
                *extension.targetType,
                *extension.targetType);
        }

        bool functionHasGenericParent(parser::FunctionDeclaration const& function) const
        {
            TypeID const receiverTypeId = receiverTypeIdForFunction(function);
            if (receiverTypeId == 0)
            {
                return false;
            }

            parser::StructDeclaration const* structDecl =
                sema::findStructDeclForType(context, receiverTypeId);
            return structDecl != nullptr && !structDecl->typeParameters.empty();
        }

        TypeID genericBodyReceiverTypeIdForReceiver(TypeID receiverTypeId) const
        {
            if (receiverTypeId == 0)
            {
                return 0;
            }

            parser::StructDeclaration const* structDecl =
                sema::findStructDeclForType(context, receiverTypeId);
            if (structDecl == nullptr || structDecl->typeParameters.empty())
            {
                return receiverTypeId;
            }

            ASTContext& mutableContext = const_cast<ASTContext&>(context);
            SemanticType const& receiverType = context.getTypes().get(receiverTypeId);
            Array<TypeID> arguments;
            arguments.reserve(structDecl->typeParameters.size());
            for (parser::TypeParameter const& parameter : structDecl->typeParameters)
            {
                IdentifierID const parameterNameId =
                    mutableContext.getIdentifiers().getOrInsert(parameter.name.lexeme);
                arguments.push_back(
                    sema::resolveCanonicalNamedType(mutableContext, parameterNameId));
            }

            return mutableContext.getTypes().internGeneric(
                receiverType.nameId,
                std::move(arguments));
        }

        TypeID genericBodyReceiverTypeIdForFunction(parser::FunctionDeclaration const& function) const
        {
            return genericBodyReceiverTypeIdForReceiver(receiverTypeIdForFunction(function));
        }

        TypeID genericBodyReceiverTypeIdForProperty(parser::PropertyDeclaration const& property) const
        {
            return genericBodyReceiverTypeIdForReceiver(receiverTypeIdForProperty(property));
        }

        String functionRILName(parser::FunctionDeclaration const& function) const
        {
            BoundDecl const* decl = findRegisteredDecl(context, function);
            if (decl != nullptr)
            {
                sema::SemanticDecl const* semanticDecl =
                    context.getSemanticDecls().findForBoundDecl(decl->id);
                if (semanticDecl != nullptr && semanticDecl->exported)
                {
                    return std::format(
                        "{}.{}",
                        context.getModuleDecl().name,
                        decl->id);
                }
            }

            if (decl != nullptr && decl->parentScopeNameId != 0)
            {
                return std::format(
                    "{}.{}",
                    context.getIdentifiers().lookup(decl->parentScopeNameId),
                    function.name.lexeme);
            }

            return function.name.lexeme;
        }

        String genericFunctionKey(
            parser::FunctionDeclaration const& function,
            sema::GenericSubstitutionMap const& substitution) const
        {
            String key = functionRILName(function);
            key += "<";
            for (usize index = 0; index < function.typeParameters.size(); ++index)
            {
                Optional<IdentifierID> const maybeNameId = context.getIdentifiers().find(
                    function.typeParameters[index].name.lexeme);
                auto const found = maybeNameId.has_value()
                                     ? substitution.find(*maybeNameId)
                                     : substitution.end();
                key += index == 0 ? "" : ",";
                key += found != substitution.end()
                         ? std::to_string(found->second)
                         : String("?");
            }
            key += ">";
            return key;
        }

        Result<uint32> ensureGenericFunctionInstance(
            parser::FunctionDeclaration const& function,
            sema::GenericSubstitutionMap const& substitution,
            Optional<TypeID> concreteReturnTypeId,
            Array<TypeID> const* concreteParameterTypeIds,
            Optional<TypeID> concreteReceiverTypeId,
            SourceLocation location,
            bool genericBodyInstantiation = false)
        {
            if (function.body == nullptr)
            {
                return SourceError("RIL builder requires generic function body", location);
            }

            String key = genericFunctionKey(function, substitution);
            if (concreteReceiverTypeId.has_value())
            {
                key += "#receiver:";
                key += std::to_string(*concreteReceiverTypeId);
            }
            if (concreteParameterTypeIds != nullptr)
            {
                key += "#parameters:";
                for (usize index = 0; index < concreteParameterTypeIds->size(); ++index)
                {
                    key += index == 0 ? "" : ",";
                    key += std::to_string((*concreteParameterTypeIds)[index]);
                }
            }
            auto const existing = genericFunctionSlots.find(key);
            if (existing != genericFunctionSlots.end())
            {
                return existing->second;
            }

            Array<IdentifierID> genericBodyParameterNameIds;
            if (genericBodyInstantiation)
            {
                genericBodyParameterNameIds.reserve(
                    substitution.size() + function.typeParameters.size());
                for (auto const& entry : substitution)
                {
                    genericBodyParameterNameIds.push_back(entry.first);
                }
                for (parser::TypeParameter const& parameter : function.typeParameters)
                {
                    if (Optional<IdentifierID> const nameId =
                            context.getIdentifiers().find(parameter.name.lexeme))
                    {
                        genericBodyParameterNameIds.push_back(*nameId);
                    }
                }
            }

            TypeID returnTypeId = 0;
            if (concreteReturnTypeId.has_value())
            {
                returnTypeId = *concreteReturnTypeId;
            }
            else
            {
                Result<TypeID> declaredReturnTypeId = functionReturnTypeId(context, function);
                if (declaredReturnTypeId.isError())
                {
                    return declaredReturnTypeId.error();
                }
                returnTypeId = sema::substituteGenericType(
                    const_cast<ASTContext&>(context),
                    std::move(declaredReturnTypeId).unwrap(),
                    substitution);
            }
            Result<Type> returnType =
                genericBodyInstantiation
                    ? genericBodyRILTypeFromSemanticType(
                          context,
                          returnTypeId,
                          genericBodyParameterNameIds,
                          location)
                    : rilTypeFromSemanticType(context, returnTypeId, location);
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            parameterTypes.reserve(
                function.parameters.size() + (concreteReceiverTypeId.has_value() ? 1u : 0u));
            if (concreteReceiverTypeId.has_value())
            {
                Result<Type> receiverType =
                    genericBodyInstantiation
                        ? genericBodyRILTypeFromSemanticType(
                              context,
                              *concreteReceiverTypeId,
                              genericBodyParameterNameIds,
                              location)
                        : rilTypeFromSemanticType(context, *concreteReceiverTypeId, location);
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                Type concreteReceiverType = std::move(receiverType).unwrap();
                parameterTypes.push_back(
                    function.isMutating
                        ? Type::reference(concreteReceiverType, true)
                        : concreteReceiverType);
            }
            for (usize parameterIndex = 0;
                 parameterIndex < function.parameters.size();
                 ++parameterIndex)
            {
                parser::Parameter const& parameter = function.parameters[parameterIndex];
                BoundDecl const* decl = context.getParameterDecl(parameter);
                Optional<TypeID> parameterTypeId =
                    decl != nullptr ? context.getDeclType(decl) : std::nullopt;
                if (!parameterTypeId.has_value())
                {
                    return SourceError(
                        "RIL builder requires generic parameter type metadata",
                        parameter.name.location);
                }
                TypeID const substitutedParameterTypeId =
                    concreteParameterTypeIds != nullptr &&
                            parameterIndex < concreteParameterTypeIds->size()
                        ? (*concreteParameterTypeIds)[parameterIndex]
                        : sema::substituteGenericType(
                              const_cast<ASTContext&>(context),
                              *parameterTypeId,
                              substitution);
                Result<Type> parameterType =
                    genericBodyInstantiation
                        ? genericBodyRILTypeFromSemanticType(
                              context,
                              substitutedParameterTypeId,
                              genericBodyParameterNameIds,
                              parameter.name.location)
                        : rilTypeFromSemanticType(
                              context,
                              substitutedParameterTypeId,
                              parameter.name.location);
                if (parameterType.isError())
                {
                    return parameterType.error();
                }
                parameterTypes.push_back(std::move(parameterType).unwrap());
            }

            FunctionBuilder builder(
                *this,
                context,
                key,
                std::move(returnType).unwrap(),
                returnTypeId,
                std::move(parameterTypes),
                false,
                concreteReceiverTypeId.value_or(0),
                false,
                substitution,
                genericBodyInstantiation,
                std::move(genericBodyParameterNameIds),
                materializedEvalValues,
                lowersEvalOperands);

            Result<Function> lowered = builder.finish(function);
            if (lowered.isError())
            {
                return lowered.error();
            }

            uint32 const slot = static_cast<uint32>(module.functions.size());
            genericFunctionSlots[key] = slot;
            module.functions.push_back(std::move(lowered).unwrap());
            return slot;
        }

        Result<Function> buildGenericBodyFunction(
            parser::FunctionDeclaration const& function,
            Array<IdentifierID> genericParameterNameIds = {})
        {
            if (function.body == nullptr)
            {
                return SourceError("RIL builder requires generic function body", function.getLocation());
            }

            TypeID const receiverTypeId = genericBodyReceiverTypeIdForFunction(function);
            TypeID returnTypeId = receiverTypeId;
            if (!function.isInitializer() || receiverTypeId == 0)
            {
                Result<TypeID> declaredReturnTypeId = functionReturnTypeId(context, function);
                if (declaredReturnTypeId.isError())
                {
                    return declaredReturnTypeId.error();
                }
                returnTypeId = std::move(declaredReturnTypeId).unwrap();
            }
            else if (function.throwsClause != nullptr)
            {
                Result<TypeID> throwingReturnTypeId = functionReturnTypeIdForRIL(function, receiverTypeId);
                if (throwingReturnTypeId.isError())
                {
                    return throwingReturnTypeId.error();
                }
                returnTypeId = std::move(throwingReturnTypeId).unwrap();
            }
            Result<Type> returnType =
                genericBodyRILTypeFromSemanticType(
                    context,
                    returnTypeId,
                    genericParameterNameIds,
                    function.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0 && !function.isInitializer())
            {
                Result<Type> receiverType =
                    genericBodyRILTypeFromSemanticType(
                        context,
                        receiverTypeId,
                        genericParameterNameIds,
                        function.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                Type receiver = std::move(receiverType).unwrap();
                parameterTypes.push_back(
                    function.isMutating ? Type::reference(std::move(receiver), true)
                                        : std::move(receiver));
            }

            for (parser::Parameter const& parameter : function.parameters)
            {
                BoundDecl const* decl = context.getParameterDecl(parameter);
                Optional<TypeID> parameterTypeId =
                    decl != nullptr ? context.getDeclType(decl) : std::nullopt;
                if (!parameterTypeId.has_value())
                {
                    return SourceError(
                        "RIL builder requires generic generic body parameter type metadata",
                        parameter.name.location);
                }

                Result<Type> parameterType =
                    genericBodyRILTypeFromSemanticType(
                        context,
                        *parameterTypeId,
                        genericParameterNameIds,
                        parameter.name.location);
                if (parameterType.isError())
                {
                    return parameterType.error();
                }
                parameterTypes.push_back(std::move(parameterType).unwrap());
            }

            FunctionBuilder builder(
                *this,
                context,
                functionRILName(function),
                std::move(returnType).unwrap(),
                returnTypeId,
                std::move(parameterTypes),
                false,
                receiverTypeId,
                function.isInitializer(),
                {},
                true,
                genericParameterNameIds,
                materializedEvalValues,
                lowersEvalOperands);

            return builder.finish(function);
        }

        Result<Function> buildGenericBodyPropertyGetter(
            parser::PropertyDeclaration const& property,
            Array<IdentifierID> genericParameterNameIds = {})
        {
            if (!property.getter.has_value() ||
                *property.getter == nullptr ||
                (*property.getter)->body == nullptr)
            {
                return SourceError(
                    "RIL builder requires generic property getter body",
                    property.getLocation());
            }

            BoundDecl const* propertyBoundDecl = findRegisteredDecl(context, property);
            Optional<TypeID> const propertyTypeId =
                propertyBoundDecl != nullptr
                    ? context.getDeclType(propertyBoundDecl)
                    : std::nullopt;
            if (!propertyTypeId.has_value())
            {
                return SourceError(
                    "RIL builder requires generic property getter type metadata",
                    property.getLocation());
            }

            Result<Type> returnType =
                genericBodyRILTypeFromSemanticType(
                    context,
                    *propertyTypeId,
                    genericParameterNameIds,
                    property.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }

            TypeID const receiverTypeId = genericBodyReceiverTypeIdForProperty(property);
            Array<Type> parameterTypes;
            if (receiverTypeId != 0)
            {
                Result<Type> receiverType =
                    genericBodyRILTypeFromSemanticType(
                        context,
                        receiverTypeId,
                        genericParameterNameIds,
                        property.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(std::move(receiverType).unwrap());
            }

            FunctionBuilder builder(
                *this,
                context,
                std::format("property.{}", property.name.lexeme),
                std::move(returnType).unwrap(),
                *propertyTypeId,
                std::move(parameterTypes),
                false,
                receiverTypeId,
                false,
                {},
                true,
                genericParameterNameIds,
                materializedEvalValues,
                lowersEvalOperands);

            return builder.finishPropertyGetter(property);
        }

        Result<uint32> ensureGenericBodyExternalFunction(
            parser::FunctionDeclaration const& function,
            Type resultType,
            TypeID receiverTypeId,
            SourceLocation location)
        {
            String key = functionRILName(function);
            key += "#generic-body-external";
            auto const existing = genericFunctionSlots.find(key);
            if (existing != genericFunctionSlots.end())
            {
                return existing->second;
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0 && !function.isInitializer())
            {
                Result<Type> receiverType =
                    genericBodyRILTypeFromSemanticType(context, receiverTypeId, location);
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                Type receiver = std::move(receiverType).unwrap();
                parameterTypes.push_back(
                    function.isMutating ? Type::reference(std::move(receiver), true)
                                        : std::move(receiver));
            }

            for (parser::Parameter const& parameter : function.parameters)
            {
                BoundDecl const* decl = context.getParameterDecl(parameter);
                Optional<TypeID> parameterTypeId =
                    decl != nullptr ? context.getDeclType(decl) : std::nullopt;
                if (!parameterTypeId.has_value())
                {
                    return SourceError(
                        "RIL builder requires generic generic body external parameter metadata",
                        parameter.name.location);
                }

                Result<Type> parameterType =
                    genericBodyRILTypeFromSemanticType(context, *parameterTypeId, parameter.name.location);
                if (parameterType.isError())
                {
                    return parameterType.error();
                }
                parameterTypes.push_back(std::move(parameterType).unwrap());
            }

            uint32 const slot = static_cast<uint32>(module.functions.size());
            genericFunctionSlots[key] = slot;
            module.functions.push_back(Function{
                .name = key,
                .returnType = std::move(resultType),
                .parameters = std::move(parameterTypes),
                .isExternal = true,
            });
            return slot;
        }

        Result<TypeID> functionReturnTypeIdForRIL(
            parser::FunctionDeclaration const& function,
            TypeID receiverTypeId) const
        {
            if (!function.isInitializer() || receiverTypeId == 0)
            {
                return functionReturnTypeId(context, function);
            }

            if (function.throwsClause == nullptr)
            {
                return receiverTypeId;
            }

            Result<TypeID> declaredReturnType = functionReturnTypeId(context, function);
            if (declaredReturnType.isError())
            {
                return declaredReturnType.error();
            }

            Optional<TypeID> errorTypeId;
            if (!sema::isResultType(context, std::move(declaredReturnType).unwrap(), nullptr, &errorTypeId) ||
                !errorTypeId.has_value())
            {
                return SourceError(
                    "RIL builder requires Result return type for throwing initializer",
                    function.getLocation());
            }

            return sema::makeResultType(
                const_cast<ASTContext&>(context),
                receiverTypeId,
                *errorTypeId);
        }

        Result<bool> registerFunction(parser::FunctionDeclaration const& function)
        {
            if (!function.typeParameters.empty() || functionHasGenericParent(function))
            {
                return true;
            }

            if (function.body == nullptr)
            {
                return SourceError("RIL builder only supports non-generic functions with bodies", function.getLocation());
            }

            TypeID const receiverTypeId = receiverTypeIdForFunction(function);
            Result<TypeID> functionReturnTypeId = functionReturnTypeIdForRIL(function, receiverTypeId);
            if (functionReturnTypeId.isError())
            {
                return functionReturnTypeId.error();
            }
            TypeID const returnTypeId = std::move(functionReturnTypeId).unwrap();
            Result<Type> returnType = rilTypeFromSemanticType(
                context,
                returnTypeId,
                function.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }
            Result<Array<Type>> parameters = functionParameterTypes(context, function);
            if (parameters.isError())
            {
                return parameters.error();
            }

            Array<Type> parameterTypes = std::move(parameters).unwrap();
            if (receiverTypeId != 0 && !function.isInitializer())
            {
                Result<Type> receiverType =
                    rilTypeFromSemanticType(context, receiverTypeId, function.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }

                Type concreteReceiverType = std::move(receiverType).unwrap();
                parameterTypes.insert(
                    parameterTypes.begin(),
                    function.isMutating
                        ? Type::reference(concreteReceiverType, true)
                        : concreteReceiverType);
            }

            Type finalReturnType = std::move(returnType).unwrap();
            String functionName =
                function.name.lexeme == names::mainFunction
                    ? String(names::mainFunction)
                    : functionRILName(function);
            uint32 const slot = static_cast<uint32>(module.functions.size());
            functionSlots[&function] = slot;
            sourceFunctionSlots[slot] = &function;
            functionTypes[&function] = finalReturnType;
            functionParameterTypeMap[&function] = parameterTypes;
            module.functions.push_back(Function{
                .name = std::move(functionName),
                .returnType = std::move(finalReturnType),
                .parameters = std::move(parameterTypes),
                .isExternal = true,
            });
            return true;
        }

        Result<bool> registerPropertyGetter(
            parser::PropertyDeclaration const& property,
            TypeID receiverTypeId)
        {
            if (!property.getter.has_value() ||
                *property.getter == nullptr ||
                (*property.getter)->body == nullptr)
            {
                return true;
            }

            BoundDecl const* propertyBoundDecl = findRegisteredDecl(context, property);
            Optional<TypeID> const propertyTypeId =
                propertyBoundDecl != nullptr
                    ? context.getDeclType(propertyBoundDecl)
                    : std::nullopt;
            if (!propertyTypeId.has_value())
            {
                return SourceError(
                    "RIL builder requires property getter type metadata",
                    property.getLocation());
            }

            Result<Type> returnType =
                rilTypeFromSemanticType(context, *propertyTypeId, property.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0)
            {
                Result<Type> receiverType =
                    rilTypeFromSemanticType(context, receiverTypeId, property.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(std::move(receiverType).unwrap());
            }

            Type finalReturnType = std::move(returnType).unwrap();
            uint32 const slot = static_cast<uint32>(module.functions.size());
            propertyGetterSlots[&property] = slot;
            sourcePropertyGetterSlots[slot] = &property;
            propertyGetterTypes[&property] = finalReturnType;
            propertyGetterParameterTypeMap[&property] = parameterTypes;
            module.functions.push_back(Function{
                .name = std::format("property.{}", slot),
                .returnType = std::move(finalReturnType),
                .parameters = std::move(parameterTypes),
                .isExternal = true,
            });
            return true;
        }

        Result<bool> buildFunction(parser::FunctionDeclaration const& function)
        {
            if (!function.typeParameters.empty() || functionHasGenericParent(function))
            {
                return true;
            }

            bool const isEntryPoint =
                &function == entryPoint ||
                (entryPoint == nullptr && function.name.lexeme == names::mainFunction);
            TypeID const receiverTypeId = receiverTypeIdForFunction(function);
            Result<TypeID> returnTypeId = functionReturnTypeIdForRIL(function, receiverTypeId);
            if (returnTypeId.isError())
            {
                return returnTypeId.error();
            }
            TypeID const loweredReturnTypeId = std::move(returnTypeId).unwrap();
            Result<Type> returnType = rilTypeFromSemanticType(
                context,
                loweredReturnTypeId,
                function.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }
            auto const parameterTypes = functionParameterTypeMap.find(&function);
            if (parameterTypes == functionParameterTypeMap.end())
            {
                return SourceError("RIL builder requires registered function parameter metadata", function.getLocation());
            }

            FunctionBuilder builder(
                *this,
                context,
                function.name.lexeme == names::mainFunction
                    ? String(names::mainFunction)
                    : functionRILName(function),
                std::move(returnType).unwrap(),
                loweredReturnTypeId,
                parameterTypes->second,
                isEntryPoint,
                receiverTypeId,
                function.isInitializer(),
                {},
                false,
                {},
                materializedEvalValues,
                lowersEvalOperands);

            Result<Function> lowered = builder.finish(function);
            if (lowered.isError())
            {
                return lowered.error();
            }

            auto const slot = functionSlots.find(&function);
            if (slot == functionSlots.end() || slot->second >= module.functions.size())
            {
                return SourceError("RIL builder requires reserved function slot", function.getLocation());
            }

            module.functions[slot->second] = std::move(lowered).unwrap();
            return true;
        }

        Result<bool> buildReachableFunctionBody(uint32 functionSlot, SourceLocation location)
        {
            if (functionSlot >= module.functions.size())
            {
                return SourceError("RIL builder call references an invalid function slot", location);
            }

            if (!module.functions[functionSlot].isExternal)
            {
                return true;
            }

            if (auto const sourceFunction = sourceFunctionSlots.find(functionSlot);
                sourceFunction != sourceFunctionSlots.end())
            {
                return buildFunction(*sourceFunction->second);
            }

            if (auto const sourceProperty = sourcePropertyGetterSlots.find(functionSlot);
                sourceProperty != sourcePropertyGetterSlots.end())
            {
                TypeID const receiverTypeId = receiverTypeIdForProperty(*sourceProperty->second);
                return buildPropertyGetter(*sourceProperty->second, receiverTypeId);
            }

            return true;
        }

        Result<bool> buildReachableFunctionBodiesFrom(uint32 functionSlot)
        {
            Array<uint32> pending;
            HashSet<uint32> visited;
            pending.push_back(functionSlot);

            while (!pending.empty())
            {
                uint32 const currentSlot = pending.back();
                pending.pop_back();
                if (visited.contains(currentSlot))
                {
                    continue;
                }
                visited.insert(currentSlot);

                Result<bool> builtCurrent = buildReachableFunctionBody(currentSlot, {});
                if (builtCurrent.isError())
                {
                    return builtCurrent.error();
                }

                if (currentSlot >= module.functions.size())
                {
                    return SourceError("RIL builder reachable function slot is invalid", {});
                }

                Array<std::pair<uint32, SourceLocation>> callTargets;
                for (Value const& value : module.functions[currentSlot].values)
                {
                    if (value.kind == ValueKind::Call)
                    {
                        callTargets.push_back({value.functionSlot, value.location});
                    }
                }

                for (auto const& [targetSlot, location] : callTargets)
                {
                    Result<bool> builtTarget = buildReachableFunctionBody(targetSlot, location);
                    if (builtTarget.isError())
                    {
                        return builtTarget.error();
                    }
                    pending.push_back(targetSlot);
                }
            }

            return true;
        }

        Result<bool> buildPropertyGetter(
            parser::PropertyDeclaration const& property,
            TypeID receiverTypeId)
        {
            if (!property.getter.has_value() ||
                *property.getter == nullptr ||
                (*property.getter)->body == nullptr)
            {
                return true;
            }

            auto const slot = propertyGetterSlots.find(&property);
            auto const returnType = propertyGetterTypes.find(&property);
            auto const parameterTypes = propertyGetterParameterTypeMap.find(&property);
            if (slot == propertyGetterSlots.end() ||
                returnType == propertyGetterTypes.end() ||
                parameterTypes == propertyGetterParameterTypeMap.end())
            {
                return SourceError(
                    "RIL builder requires registered property getter metadata",
                    property.getLocation());
            }

            BoundDecl const* propertyBoundDecl = findRegisteredDecl(context, property);
            Optional<TypeID> const propertyTypeId =
                propertyBoundDecl != nullptr
                    ? context.getDeclType(propertyBoundDecl)
                    : std::nullopt;
            if (!propertyTypeId.has_value())
            {
                return SourceError(
                    "RIL builder requires property getter type metadata",
                    property.getLocation());
            }

            FunctionBuilder builder(
                *this,
                context,
                std::format("property.{}", slot->second),
                returnType->second,
                *propertyTypeId,
                parameterTypes->second,
                false,
                receiverTypeId,
                false,
                {},
                false,
                {},
                materializedEvalValues,
                lowersEvalOperands);

            Result<Function> lowered = builder.finishPropertyGetter(property);
            if (lowered.isError())
            {
                return lowered.error();
            }

            if (slot->second >= module.functions.size())
            {
                return SourceError("RIL builder requires reserved property getter slot", property.getLocation());
            }

            module.functions[slot->second] = std::move(lowered).unwrap();
            return true;
        }
    };

}
