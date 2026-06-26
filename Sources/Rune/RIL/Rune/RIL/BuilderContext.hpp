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
        bool hasBreak{false};
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
        HashMap<BoundDecl const*, uint32> transientSlots;
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
        bool serializedBodyMode{false};
        bool lowersEvalOperand{false};
        bool wrapReturnsInResult{false};
        Optional<uint32> initializerReceiverReferenceOverride;
        Array<IdentifierID> serializedBodyGenericParameterNameIds;
        uint32 currentBlock{0};
        // Expressions currently being wrapped into an `Any` box. Guards the
        // implicit-widening hook in `buildExpression` from re-entering when it
        // rebuilds the same node to obtain the inner (unboxed) value.
        HashSet<parser::Expression const*> anyBoxingInProgress;

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
            bool serializedBodyMode = false,
            Array<IdentifierID> serializedBodyGenericParameterNameIds = {},
            MaterializedEvalValues const* materializedEvalValues = nullptr,
            bool lowersEvalOperand = false,
            bool wrapReturnsInResult = false);

        BasicBlock& block();

        uint32 addBlock(String name);

        Result<uint32> buildExpression(
            parser::Expression const& expression,
            Optional<Type> expectedType = std::nullopt);
        /// Builds `expression` coerced to the option type `optionTypeId`,
        /// wrapping a bare payload value (including a reference) into the option
        /// union when needed. When the target is not an option, or the
        /// expression already has the option type, this is equivalent to
        /// buildExpression with the option type as the expected type.
        Result<uint32> buildExpressionCoercedToOption(
            parser::Expression const& expression,
            TypeID optionTypeId,
            Type const& optionType);
        uint32 addStringLiteralValue(String value, Type type = Type::stringType());
        Result<uint32> buildTryUnwrappedExpression(
            parser::Expression const& expression,
            uint32 resultValue,
            Optional<Type> expectedType = std::nullopt);
        Result<uint32> buildLambdaExpression(parser::LambdaExpression const& lambda);
        Result<uint32> buildOptionalPrimitiveCastExpression(
            parser::CastExpression const& cast,
            sema::PrimitiveConversion const& conversion);
        /// Lowers a downcast on a fat reference (`*OpenBase as? Derived` /
        /// `as! Derived`) against a closed target struct. Returns an empty
        /// Optional when `cast` is not such a downcast (so the caller can fall
        /// back to its normal error path).
        Result<Optional<uint32>> buildStructReferenceDowncast(
            parser::CastExpression const& cast,
            parser::Expression const& expression);
        /// Lowers `anyValue as? T` / `anyValue as! T`: compares the box's stored
        /// type id against `T`'s introspection id, then extracts the inline value
        /// (`as?` yields `T?`, `as!` traps on a mismatch).
        Result<uint32> buildAnyDowncast(
            parser::CastExpression const& cast,
            parser::Expression const& expression);
        /// Builds a fat reference aggregate `{ data, witnessTable }` for a value
        /// of `concreteTypeId` viewed through a base reference whose pointee is
        /// `fatReferencePointeeTypeId` (an interface or `open` struct). `dataValue`
        /// is the pointer to the concrete storage.
        Result<uint32> buildFatReferenceValue(
            TypeID fatReferencePointeeTypeId,
            TypeID concreteTypeId,
            uint32 dataValue,
            Type const& resultType,
            SourceLocation location);
        /// Wraps `expression` into an `Any` existential box. The inner value is
        /// built with its natural type, then `AnyBox` writes the concrete type's
        /// introspection id and inline value into the 32-byte box. Content that
        /// does not fit the inline buffer or owns heap storage is rejected.
        Result<uint32> buildAnyBoxConversion(
            parser::Expression const& expression,
            TypeID innerTypeId);
        Result<uint32> buildAddressOfExpression(parser::Expression const& expression);
        Result<uint32> buildMethodReceiverArgument(
            parser::Expression const& expression,
            Type const& parameterType);
        Result<uint32> buildCFunctionValue(
            parser::Expression const& expression,
            TypeID callbackTypeId,
            SourceLocation location);
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
        // Resolves a deferred ConstrainedSpaceship intrinsic (emitted into a
        // serialized generic body while its operand's type was still abstract)
        // now that the generic parameter is concrete: to a native PrimitiveCompare
        // for a primitive operand, or a call to the type's `<=>` witness for a
        // struct. No-op for any other value.
        Result<bool> resolveConstrainedComparisonIntrinsic(
            Value& value,
            SourceLocation location);
        // Handles a call to an interface requirement on a receiver of constrained
        // generic type `T extends I`. Returns the built value, or nullopt when the
        // call is not such a constrained-generic method call (letting the normal
        // call lowering proceed). A concrete receiver calls the type's witness
        // directly; an abstract one (serialized-body generation) emits a deferred
        // ConstrainedWitnessCall intrinsic that instantiation resolves.
        Result<Optional<uint32>> buildConstrainedInterfaceMethodCall(
            parser::CallExpression const& call,
            parser::Expression const& expression,
            InterfaceDeclReference const* exported,
            Type const& callResultType);
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
        Result<bool> emitLocalFinalizers(
            usize startIndex,
            Array<uint32> const& excludedSlots = {});
        Result<bool> emitOnlyLocalFinalizers(Array<uint32> const& includedSlots);
        Result<bool> buildVariableDeclaration(parser::VariableDeclaration const& declaration);
        Result<bool> buildAssignment(parser::AssignExpression const& assignment);
        Result<bool> buildReferenceAssignment(parser::BinaryExpression const& assignment);
        Result<bool> buildCompoundAssignment(parser::BinaryExpression const& assignment);
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
        Result<Optional<uint32>> buildInitializerExpressionIntoReference(
            parser::Expression const& expression,
            uint32 receiverReference,
            Optional<Type> expectedType = std::nullopt);
        Result<uint32> buildInitializedBufferValue(
            Type const& bufferType,
            Type const& elementType,
            SourceLocation location);
        Result<uint32> buildInitializedArrayValue(
            Type const& arrayType,
            Type const& elementType,
            SourceLocation location);
        Result<uint32> buildContiguousValueFromElements(
            Type const& aggregateType,
            Type const& elementType,
            Array<parser::Expression const*> const& elements,
            SourceLocation location,
            Optional<TypeID> semanticElementTypeId = std::nullopt);
        Result<uint32> buildContiguousValueFromValues(
            Type const& aggregateType,
            Type const& elementType,
            Array<uint32> const& values,
            SourceLocation location);
        Result<Function> finish(parser::FunctionDeclaration const& declaration);
        Result<Function> finishEvaluationExpression(parser::Expression const& expression);
        Result<Function> finishDefaultArgument(
            parser::FunctionDeclaration const& declaration,
            usize parameterIndex);
        Result<Function> finishLambda(parser::LambdaExpression const& lambda);
        Result<Function> finishPropertyGetter(parser::PropertyDeclaration const& declaration);
        void buildForeignAbiSignatures();
    };

    struct ModuleBuilder final
    {
        struct GlobalInitializer final
        {
            parser::VariableDeclaration const* declaration{nullptr};
            uint32 globalSlot{0};
            Type type{Type::voidType()};
        };

        ASTContext const& context;
        parser::FunctionDeclaration const* entryPoint;
        MaterializedEvalValues const* materializedEvalValues{nullptr};
        Module module;
        bool serializedBodyGlobalsRegistered{false};
        HashMap<BoundDecl const*, uint32> globalSlots;
        HashMap<String, uint32> importedGlobalSlots;
        Array<GlobalInitializer> globalInitializers;
        HashMap<parser::FunctionDeclaration const*, uint32> functionSlots;
        HashMap<uint32, parser::FunctionDeclaration const*> sourceFunctionSlots;
        HashMap<parser::FunctionDeclaration const*, Type> functionTypes;
        HashMap<parser::FunctionDeclaration const*, Array<Type>> functionParameterTypeMap;
        HashMap<parser::PropertyDeclaration const*, uint32> propertyGetterSlots;
        HashMap<uint32, parser::PropertyDeclaration const*> sourcePropertyGetterSlots;
        HashMap<parser::PropertyDeclaration const*, Type> propertyGetterTypes;
        HashMap<parser::PropertyDeclaration const*, Array<Type>> propertyGetterParameterTypeMap;
        HashMap<parser::FunctionDeclaration const*, uint32> functionValueAdapterSlots;
        HashMap<parser::FunctionDeclaration const*, uint32> foreignCallbackAdapterSlots;
        Optional<uint32> moduleInitializerSlot;
        HashMap<String, uint32> genericFunctionSlots;
        HashMap<String, uint32> importedFunctionSlots;
        HashMap<uint32, Array<Optional<uint32>>> importedDefaultArgumentSlots;
        HashMap<String, uint32> runtimeHashCallbackSlots;
        HashMap<String, uint32> runtimeEqualCallbackSlots;
        HashMap<TypeID, Type> rilTypeBySemanticTypeId;
        uint32 lambdaIndex{0};
        bool lowersEvalOperands{false};
        /// True when this builder exists solely to generate serialized
        /// implementation bodies (see Package/Writer.cpp). Every function it
        /// instantiates is therefore part of a serialized body, so
        /// ensureGenericFunctionInstance treats all instantiations as
        /// serialized-mode regardless of the call site that requested them --
        /// this is what keeps transitively-reached calls (operators, static
        /// factories) on the serialized-body lowering path instead of the
        /// direct-module-call path, which requires pre-registered slots that a
        /// per-declaration serialized-body builder never has.
        bool serializedBodyGeneration{false};

        ModuleBuilder(ASTContext const& context, parser::FunctionDeclaration const* entryPoint);

        /// Cached Sema→RIL type conversion for this module build.
        Result<Type> rilType(TypeID typeId, SourceLocation location)
        {
            if (typeId != 0)
            {
                auto const cached = rilTypeBySemanticTypeId.find(typeId);
                if (cached != rilTypeBySemanticTypeId.end())
                {
                    return cached->second;
                }
            }

            Result<Type> type = rilTypeFromSemanticType(context, typeId, location);
            if (type.isError() || typeId == 0)
            {
                return type;
            }

            Type const& value = type.value();
            rilTypeBySemanticTypeId.emplace(typeId, value);
            return value;
        }

        Result<bool> registerProgramDeclarations(parser::Program const& program);
        Result<bool> buildProgramFunctionBodies(parser::Program const& program);
        Result<bool> registerDeclaration(parser::Statement const& declaration);
        Result<bool> buildDeclarationFunctionBodies(parser::Statement const& declaration);
        Result<bool> buildModuleInitializerFunction();

        Result<uint32> ensureImportedFunction(
            parser::CallExpression const& call,
            InterfaceDeclReference const& interfaceRef,
            Type resultType);
        Result<uint32> ensureImportedFunctionForReceiver(
            InterfaceDeclReference const& interfaceRef,
            TypeID callerReceiverTypeId,
            Type resultType,
            SourceLocation location,
            sema::GenericSubstitutionMap const* callGenericSubstitutions = nullptr);
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

        uint32 importMaterializedStaticObject(MaterializedStaticObject const& object)
        {
            uint32 const slotOffset = static_cast<uint32>(module.staticData.size());
            for (StaticData const& source : object.staticData)
            {
                StaticData data = source;
                module.staticData.push_back(std::move(data));
            }

            for (uint32 index = 0; index < object.staticData.size(); ++index)
            {
                StaticData& data = module.staticData[slotOffset + index];
                for (StaticData::Relocation& relocation : data.relocations)
                {
                    relocation.targetSlot += slotOffset;
                }
            }

            return slotOffset + object.rootStaticDataSlot;
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

            if (Optional<EnumCaseValue> const enumCase = context.getEnumCaseValue(expression))
            {
                Result<Type> type = expectedType.has_value()
                                      ? Result<Type>(*expectedType)
                                      : rilType(
                                            
                                            enumCase->rawTypeId,
                                            expression.getLocation());
                if (type.isError())
                {
                    return type.error();
                }

                return module.addValue(Value::integer(
                    enumCase->rawValue,
                    std::move(type).unwrap()));
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
                if (module.globals[slot->second].requiresRuntimeInitialization)
                {
                    return SourceError(
                        "RIL module constant depends on a runtime-initialized global",
                        expression.getLocation());
                }
                return module.addValue(Value::globalLoad(
                    slot->second,
                    module.globals[slot->second].type));
            }

            if (auto const* call = parser::ast_cast<parser::CallExpression>(&expression))
            {
                auto const* callee =
                    call->callee != nullptr
                        ? parser::ast_cast<parser::VariableExpression>(call->callee.get())
                        : nullptr;
                if (callee != nullptr && callee->nameKind == parser::VariableNameKind::Macro)
                {
                    Optional<IdentifierID> const nameId =
                        context.getIdentifiers().find(callee->name.lexeme);
                    Optional<sema::BuiltinMacroDescriptor> const macro =
                        nameId.has_value()
                            ? sema::lookupBuiltinMacro(context, *nameId)
                            : std::nullopt;
                    if (macro.has_value() &&
                        macro->argumentKind == sema::BuiltinMacroArgumentKind::SourcePath)
                    {
                        ASTContext::EmbedPayload const* payload = context.getEmbedPayload(*call);
                        if (payload == nullptr)
                        {
                            return SourceError(
                                "RIL builder requires embed macro payload metadata",
                                expression.getLocation());
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

                        Type embedType = std::move(type).unwrap();
                        if (macro->kind == sema::BuiltinMacroKind::EmbedString)
                        {
                            String string = payload->bytes;
                            uint32 const data = addStringLiteralStaticData(string);
                            Value value = Value::string(std::move(string), data);
                            value.type = std::move(embedType);
                            return module.addValue(std::move(value));
                        }

                        auto const isByteType = [](Type const& byteType) -> bool
                        {
                            return byteType.kind == TypeKind::Integer &&
                                   byteType.bitWidth == 8 &&
                                   !byteType.isSigned;
                        };

                        if (embedType.kind == TypeKind::InlineArray)
                        {
                            Type const elementType = arrayElementType(embedType);
                            if (!isByteType(elementType))
                            {
                                return SourceError(
                                    "RIL builder requires #embedBytes inline array element type to be Byte",
                                    expression.getLocation());
                            }
                            if (embedType.elementCount != payload->bytes.size())
                            {
                                return SourceError(
                                    "RIL builder #embedBytes inline array element count mismatch",
                                    expression.getLocation());
                            }

                            Array<uint32> elements;
                            elements.reserve(payload->bytes.size());
                            for (char const character : payload->bytes)
                            {
                                elements.push_back(module.addValue(Value::integer(
                                    static_cast<uint8>(character),
                                    elementType)));
                            }
                            return module.addValue(Value::makeAggregate(
                                std::move(embedType),
                                std::move(elements)));
                        }

                        if (embedType.kind == TypeKind::Slice)
                        {
                            Type const elementType = arrayElementType(embedType);
                            if (!isByteType(elementType))
                            {
                                return SourceError(
                                    "RIL builder requires #embedBytes slice element type to be Byte",
                                    expression.getLocation());
                            }

                            Array<uint8> bytes;
                            bytes.reserve(payload->bytes.size());
                            for (char const character : payload->bytes)
                            {
                                bytes.push_back(static_cast<uint8>(character));
                            }

                            uint32 dataPointer = module.addValue(Value::nullPointer(embedType.aggregateFields[0]));
                            if (!bytes.empty())
                            {
                                uint32 const dataSlot = addReadonlyStaticData(
                                    std::format("__rune_static_data_{}", module.staticData.size()),
                                    std::move(bytes),
                                    1,
                                    Type::unsignedInteger(8));
                                dataPointer = module.addValue(Value::staticDataPointer(
                                    embedType.aggregateFields[0],
                                    dataSlot));
                            }

                            uint32 const count = module.addValue(Value::integer(
                                payload->bytes.size(),
                                embedType.aggregateFields[1]));
                            uint32 const stride = module.addValue(Value::integer(
                                1,
                                embedType.aggregateFields[2]));
                            return module.addValue(Value::makeAggregate(
                                embedType,
                                {dataPointer, count, stride}));
                        }
                    }
                }
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

        Result<uint32> buildModuleDefaultValue(Type const& type, SourceLocation location)
        {
            switch (type.kind)
            {
                case TypeKind::Integer:
                    return module.addValue(Value::integer(0, type));
                case TypeKind::Float:
                    return module.addValue(Value::floating(0.0, type));
                case TypeKind::Bool:
                    return module.addValue(Value::boolean(false));
                case TypeKind::Reference:
                case TypeKind::Pointer:
                case TypeKind::CStringSlice:
                case TypeKind::Function:
                    return module.addValue(Value::nullPointer(type));
                case TypeKind::Union:
                    return module.addValue(Value::makeUnion(type, 0));
                case TypeKind::String:
                    return module.addValue(Value::string(String(), addStringLiteralStaticData({})));
                case TypeKind::Tuple:
                case TypeKind::Aggregate:
                case TypeKind::Array:
                case TypeKind::Buffer:
                case TypeKind::Slice:
                case TypeKind::StringSlice:
                case TypeKind::Range:
                case TypeKind::Iterator:
                case TypeKind::GenericNominal:
                case TypeKind::VariadicPack:
                case TypeKind::Closure:
                {
                    // These are all struct-shaped at the storage level (a growable
                    // Array/Buffer is {data pointer, count, allocator, ...}, a closure
                    // is {context, function}, etc). This is the registration-time
                    // placeholder. The later fixed-point initializer pass either
                    // replaces it with a common backend static representation or
                    // retains executable initialization against zeroed storage.
                    Array<Type> const& fields =
                        type.kind == TypeKind::Tuple ? type.tupleElements : type.aggregateFields;
                    Array<uint32> values;
                    values.reserve(fields.size());
                    for (Type const& field : fields)
                    {
                        Result<uint32> value = buildModuleDefaultValue(field, location);
                        if (value.isError())
                        {
                            return value.error();
                        }
                        values.push_back(std::move(value).unwrap());
                    }
                    return module.addValue(
                        type.kind == TypeKind::Tuple
                            ? Value::makeTuple(type, std::move(values))
                            : Value::makeAggregate(type, std::move(values)));
                }
                case TypeKind::InlineArray:
                {
                    Type elementType = arrayElementType(type);
                    Array<uint32> values;
                    values.reserve(type.elementCount);
                    for (uint32 index = 0; index < type.elementCount; ++index)
                    {
                        Result<uint32> value = buildModuleDefaultValue(elementType, location);
                        if (value.isError())
                        {
                            return value.error();
                        }
                        values.push_back(std::move(value).unwrap());
                    }
                    return module.addValue(Value::makeAggregate(type, std::move(values)));
                }
                default:
                    return SourceError(
                        std::format(
                            "RIL builder cannot default-initialize global type {} yet",
                            describeType(type)),
                        location);
            }
        }

        Result<bool> registerGlobal(parser::VariableDeclaration const& declaration)
        {
            if (declaration.isTupleDestructuring())
            {
                return SourceError("RIL builder requires simple global declarations", declaration.getLocation());
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

            Result<Type> type = rilType(*typeId, declaration.getLocation());
            if (type.isError())
            {
                return type.error();
            }
            Type globalType = std::move(type).unwrap();

            Result<uint32> initializer = buildModuleDefaultValue(globalType, declaration.getLocation());
            if (initializer.isError())
            {
                return initializer.error();
            }

            uint32 const slot = module.addGlobal(Global{
                .name = declaration.name.lexeme,
                .type = globalType,
                .initializerValue = std::move(initializer).unwrap(),
                .isMutable = declaration.variableKind == parser::VariableKind::Var,
                .requiresRuntimeInitialization = declaration.initializer != nullptr,
            });
            globalSlots[decl] = slot;
            if (declaration.initializer != nullptr)
            {
                globalInitializers.push_back(GlobalInitializer{
                    .declaration = &declaration,
                    .globalSlot = slot,
                    .type = globalType,
                });
            }
            return true;
        }

        TypeID receiverTypeIdForDecl(BoundDecl const* decl) const
        {
            if (decl == nullptr)
            {
                return 0;
            }

            // An extension target can be structural (`&Handle?`, a tuple, or a
            // specialized generic). parentScopeNameId only retains its nominal
            // spelling and therefore cannot reconstruct that type. Resolve the
            // extension declaration's complete checked target before considering
            // the nominal-scope fast paths below.
            if (decl->contextKind == DeclContextKind::Extension &&
                decl->parentContext != nullptr &&
                decl->parentContext->parseNode != nullptr &&
                decl->parentContext->parseNode->getKind() == parser::ASTNodeKind::ExtensionDeclaration)
            {
                return receiverTypeIdForExtension(
                    static_cast<parser::ExtensionDeclaration const&>(
                        *decl->parentContext->parseNode));
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

        TypeID serializedBodyReceiverTypeIdForReceiver(TypeID receiverTypeId) const
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

        TypeID serializedBodyReceiverTypeIdForFunction(parser::FunctionDeclaration const& function) const
        {
            return serializedBodyReceiverTypeIdForReceiver(receiverTypeIdForFunction(function));
        }

        TypeID serializedBodyReceiverTypeIdForProperty(parser::PropertyDeclaration const& property) const
        {
            return serializedBodyReceiverTypeIdForReceiver(receiverTypeIdForProperty(property));
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

        bool functionHasForeignBinding(parser::FunctionDeclaration const& function) const
        {
            BoundDecl const* decl = findRegisteredDecl(context, function);
            if (decl == nullptr)
            {
                return false;
            }

            sema::SemanticDecl const* semanticDecl =
                context.getSemanticDecls().findForBoundDecl(decl->id);
            return semanticDecl != nullptr &&
                   context.findForeignBindingBySemanticDecl(semanticDecl->id) != nullptr;
        }

        Result<uint32> ensureGenericFunctionInstance(
            parser::FunctionDeclaration const& function,
            sema::GenericSubstitutionMap const& substitution,
            Optional<TypeID> concreteReturnTypeId,
            Array<TypeID> const* concreteParameterTypeIds,
            Optional<TypeID> concreteReceiverTypeId,
            SourceLocation location,
            bool serializedBodyInstantiation = false)
        {
            // A builder dedicated to serialized-body generation instantiates
            // every function as part of a serialized body, whichever call site
            // reached it (regular calls, operators, static factories).
            serializedBodyInstantiation =
                serializedBodyInstantiation || serializedBodyGeneration;
            if (function.body == nullptr)
            {
                if (serializedBodyInstantiation && functionHasForeignBinding(function))
                {
                    TypeID const declaredReceiverTypeId = receiverTypeIdForFunction(function);
                    TypeID const externalReceiverTypeId =
                        concreteReceiverTypeId.value_or(declaredReceiverTypeId);
                    TypeID externalReturnTypeId = concreteReturnTypeId.value_or(0);
                    if (externalReturnTypeId == 0)
                    {
                        Result<TypeID> declaredReturnType =
                            functionReturnTypeIdForRIL(function, externalReceiverTypeId);
                        if (declaredReturnType.isError())
                        {
                            return declaredReturnType.error();
                        }
                        externalReturnTypeId = std::move(declaredReturnType).unwrap();
                    }

                    Result<Type> externalReturnType =
                        serializedBodyRILTypeFromSemanticType(
                            context,
                            externalReturnTypeId,
                            location);
                    if (externalReturnType.isError())
                    {
                        return externalReturnType.error();
                    }

                    return ensureSerializedBodyExternalFunction(
                        function,
                        std::move(externalReturnType).unwrap(),
                        externalReceiverTypeId,
                        location);
                }

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
            // Serialized-body lowers keep symbolic generic types; concrete
            // module lowers do not. Keep those specializations in separate
            // cache entries so one ModuleBuilder can produce both.
            if (serializedBodyInstantiation)
            {
                key += "#serialized";
            }
            auto const existing = genericFunctionSlots.find(key);
            if (existing != genericFunctionSlots.end())
            {
                return existing->second;
            }

            Array<IdentifierID> serializedBodyGenericParameterNameIds;
            if (serializedBodyInstantiation)
            {
                serializedBodyGenericParameterNameIds.reserve(
                    substitution.size() + function.typeParameters.size());
                for (auto const& entry : substitution)
                {
                    serializedBodyGenericParameterNameIds.push_back(entry.first);
                }
                for (parser::TypeParameter const& parameter : function.typeParameters)
                {
                    if (Optional<IdentifierID> const nameId =
                            context.getIdentifiers().find(parameter.name.lexeme))
                    {
                        serializedBodyGenericParameterNameIds.push_back(*nameId);
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
                serializedBodyInstantiation
                    ? serializedBodyRILTypeFromSemanticType(
                          context,
                          returnTypeId,
                          serializedBodyGenericParameterNameIds,
                          location)
                    : rilType(returnTypeId, location);
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            bool const constructsReceiver =
                function.isInitializer() && concreteReceiverTypeId.has_value();
            parameterTypes.reserve(
                function.parameters.size() +
                (concreteReceiverTypeId.has_value() && !constructsReceiver ? 1u : 0u));
            if (concreteReceiverTypeId.has_value() && !constructsReceiver)
            {
                Result<Type> receiverType =
                    serializedBodyInstantiation
                        ? serializedBodyRILTypeFromSemanticType(
                              context,
                              *concreteReceiverTypeId,
                              serializedBodyGenericParameterNameIds,
                              location)
                        : rilType(*concreteReceiverTypeId, location);
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    function.isMutating));
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
                    serializedBodyInstantiation
                        ? serializedBodyRILTypeFromSemanticType(
                              context,
                              substitutedParameterTypeId,
                              serializedBodyGenericParameterNameIds,
                              parameter.name.location)
                        : rilType(substitutedParameterTypeId,
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
                constructsReceiver,
                substitution,
                serializedBodyInstantiation,
                std::move(serializedBodyGenericParameterNameIds),
                materializedEvalValues,
                lowersEvalOperands,
                function.throwsClause != nullptr);

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

        Result<bool> registerSerializedBodyGlobals()
        {
            if (serializedBodyGlobalsRegistered)
            {
                return true;
            }

            auto registerModuleStorageGlobals = [&](DeclContext const& scope) -> Result<bool>
            {
                for (DeclID const memberId : scope.members)
                {
                    BoundDecl const* declaration = context.getDeclRegistry().findDecl(memberId);
                    if (declaration == nullptr ||
                        declaration->parseNode == nullptr ||
                        declaration->parseNode->getKind() !=
                            parser::ASTNodeKind::VariableDeclaration ||
                        globalSlots.contains(declaration))
                    {
                        continue;
                    }

                    auto const& variable =
                        static_cast<parser::VariableDeclaration const&>(
                            *declaration->parseNode);
                    if (variable.initializer == nullptr)
                    {
                        // Declaration-only builtin constants are compiler-provided
                        // values, not module storage.
                        continue;
                    }

                    Result<bool> registered = registerGlobal(variable);
                    if (registered.isError())
                    {
                        return registered.error();
                    }
                }

                return true;
            };

            DeclContext const& moduleScope = context.getDeclRegistry().getModuleContext();
            if (Result<bool> registered = registerModuleStorageGlobals(moduleScope);
                registered.isError())
            {
                return registered.error();
            }

            for (DeclContext const* child : moduleScope.children)
            {
                if (child == nullptr || child->kind != DeclContextKind::SourceFile)
                {
                    continue;
                }

                if (Result<bool> registered = registerModuleStorageGlobals(*child);
                    registered.isError())
                {
                    return registered.error();
                }
            }

            serializedBodyGlobalsRegistered = true;
            return true;
        }

        Result<Function> buildSerializedBodyFunction(
            parser::FunctionDeclaration const& function,
            Array<IdentifierID> genericParameterNameIds = {},
            Array<TypeID> const* signatureParameterTypeIds = nullptr)
        {
            if (function.body == nullptr)
            {
                return SourceError("RIL builder requires serialized function body", function.getLocation());
            }

            Result<bool> registeredGlobals = registerSerializedBodyGlobals();
            if (registeredGlobals.isError())
            {
                return registeredGlobals.error();
            }

            TypeID const receiverTypeId = serializedBodyReceiverTypeIdForFunction(function);
            TypeID returnTypeId = receiverTypeId;
            if (!function.isInitializer() || receiverTypeId == 0)
            {
                Result<TypeID> loweredReturnTypeId = functionReturnTypeIdForRIL(function, receiverTypeId);
                if (loweredReturnTypeId.isError())
                {
                    return loweredReturnTypeId.error();
                }
                returnTypeId = std::move(loweredReturnTypeId).unwrap();
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
                serializedBodyRILTypeFromSemanticType(
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
                    serializedBodyRILTypeFromSemanticType(
                        context,
                        receiverTypeId,
                        genericParameterNameIds,
                        function.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    function.isMutating));
            }

            Array<TypeID> parameterTypeIds;
            if (signatureParameterTypeIds != nullptr)
            {
                parameterTypeIds = *signatureParameterTypeIds;
            }
            else
            {
                BoundDecl const* functionDecl = findRegisteredDecl(context, function);
                Optional<TypeID> const signatureTypeId =
                    functionDecl != nullptr ? context.getDeclSignatureType(functionDecl)
                                            : std::nullopt;
                if (!signatureTypeId.has_value())
                {
                    return SourceError(
                        "RIL builder requires serialized body function signature metadata",
                        function.getLocation());
                }
                SemanticType const& signatureType = context.getTypes().get(*signatureTypeId);
                if (signatureType.kind != SemanticTypeKind::Function)
                {
                    return SourceError(
                        "RIL builder requires serialized body function signature metadata",
                        function.getLocation());
                }
                parameterTypeIds = signatureType.parameterTypes;
            }

            if (parameterTypeIds.size() != function.parameters.size())
            {
                return SourceError(
                    "RIL builder requires serialized body function parameter signature metadata",
                    function.getLocation());
            }

            for (usize parameterIndex = 0;
                 parameterIndex < function.parameters.size();
                 ++parameterIndex)
            {
                parser::Parameter const& parameter = function.parameters[parameterIndex];

                Result<Type> parameterType =
                    serializedBodyRILTypeFromSemanticType(
                        context,
                        parameterTypeIds[parameterIndex],
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
                lowersEvalOperands,
                function.throwsClause != nullptr);

            return builder.finish(function);
        }

        Result<Function> buildSerializedDefaultArgument(
            parser::FunctionDeclaration const& function,
            usize parameterIndex,
            Array<IdentifierID> genericParameterNameIds,
            Array<TypeID> const& signatureParameterTypeIds)
        {
            if (parameterIndex >= function.parameters.size() ||
                parameterIndex >= signatureParameterTypeIds.size() ||
                function.parameters[parameterIndex].defaultValue == nullptr)
            {
                return SourceError(
                    "RIL builder requires serialized default argument metadata",
                    function.getLocation());
            }

            Result<bool> registeredGlobals = registerSerializedBodyGlobals();
            if (registeredGlobals.isError())
            {
                return registeredGlobals.error();
            }

            TypeID const receiverTypeId = serializedBodyReceiverTypeIdForFunction(function);
            TypeID const returnTypeId = signatureParameterTypeIds[parameterIndex];
            Result<Type> returnType = serializedBodyRILTypeFromSemanticType(
                context,
                returnTypeId,
                genericParameterNameIds,
                function.parameters[parameterIndex].name.location);
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0)
            {
                Result<Type> receiverType = serializedBodyRILTypeFromSemanticType(
                    context,
                    receiverTypeId,
                    genericParameterNameIds,
                    function.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    function.isMutating));
            }

            for (usize index = 0; index < parameterIndex; ++index)
            {
                Result<Type> parameterType = serializedBodyRILTypeFromSemanticType(
                    context,
                    signatureParameterTypeIds[index],
                    genericParameterNameIds,
                    function.parameters[index].name.location);
                if (parameterType.isError())
                {
                    return parameterType.error();
                }
                parameterTypes.push_back(std::move(parameterType).unwrap());
            }

            FunctionBuilder builder(
                *this,
                context,
                std::format("{}.default.{}", functionRILName(function), parameterIndex),
                std::move(returnType).unwrap(),
                returnTypeId,
                std::move(parameterTypes),
                false,
                receiverTypeId,
                false,
                {},
                true,
                std::move(genericParameterNameIds),
                materializedEvalValues,
                lowersEvalOperands);
            return builder.finishDefaultArgument(function, parameterIndex);
        }

        Result<Function> buildSerializedBodyPropertyGetter(
            parser::PropertyDeclaration const& property,
            Array<IdentifierID> genericParameterNameIds = {})
        {
            if (!property.getter.has_value() ||
                *property.getter == nullptr ||
                (*property.getter)->body == nullptr)
            {
                return SourceError(
                    "RIL builder requires serialized property getter body",
                    property.getLocation());
            }

            Result<bool> registeredGlobals = registerSerializedBodyGlobals();
            if (registeredGlobals.isError())
            {
                return registeredGlobals.error();
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
                serializedBodyRILTypeFromSemanticType(
                    context,
                    *propertyTypeId,
                    genericParameterNameIds,
                    property.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }

            TypeID const receiverTypeId = serializedBodyReceiverTypeIdForProperty(property);
            Array<Type> parameterTypes;
            if (receiverTypeId != 0)
            {
                Result<Type> receiverType =
                    serializedBodyRILTypeFromSemanticType(
                        context,
                        receiverTypeId,
                        genericParameterNameIds,
                        property.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    false));
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

        Result<uint32> ensureSerializedBodyExternalFunction(
            parser::FunctionDeclaration const& function,
            Type resultType,
            TypeID receiverTypeId,
            SourceLocation location)
        {
            String key = functionRILName(function);
            key += "#serialized-body-external";
            auto const existing = genericFunctionSlots.find(key);
            if (existing != genericFunctionSlots.end())
            {
                return existing->second;
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0 && !function.isInitializer())
            {
                Result<Type> receiverType =
                    serializedBodyRILTypeFromSemanticType(context, receiverTypeId, location);
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    function.isMutating));
            }

            for (parser::Parameter const& parameter : function.parameters)
            {
                BoundDecl const* decl = context.getParameterDecl(parameter);
                Optional<TypeID> parameterTypeId =
                    decl != nullptr ? context.getDeclType(decl) : std::nullopt;
                if (!parameterTypeId.has_value())
                {
                    return SourceError(
                        "RIL builder requires serialized body external parameter metadata",
                        parameter.name.location);
                }

                Result<Type> parameterType =
                    serializedBodyRILTypeFromSemanticType(context, *parameterTypeId, parameter.name.location);
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
            Result<Type> returnType = rilType(returnTypeId,
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
                    rilType(receiverTypeId, function.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }

                parameterTypes.insert(
                    parameterTypes.begin(),
                    methodReceiverParameterType(
                        std::move(receiverType).unwrap(),
                        function.isMutating));
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
                rilType(*propertyTypeId, property.getLocation());
            if (returnType.isError())
            {
                return returnType.error();
            }

            Array<Type> parameterTypes;
            if (receiverTypeId != 0)
            {
                Result<Type> receiverType =
                    rilType(receiverTypeId, property.getLocation());
                if (receiverType.isError())
                {
                    return receiverType.error();
                }
                parameterTypes.push_back(methodReceiverParameterType(
                    std::move(receiverType).unwrap(),
                    false));
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
            Result<Type> returnType = rilType(loweredReturnTypeId,
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
