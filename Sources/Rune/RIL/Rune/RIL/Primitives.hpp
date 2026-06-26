#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Sema/Intrinsics.hpp"
#include "Rune/Sema/SemanticDecl.hpp"

namespace rune::ril
{
    enum class TypeKind
    {
        Void,
        Integer,
        Float,
        Bool,
        Float32x4,
        InterfaceReference,
        StructReference,
        String,
        StringSlice,
        CStringSlice,
        Function,
        Closure,
        Tuple,
        Aggregate,
        Range,
        Buffer,
        Array,
        Slice,
        InlineArray,
        VariadicPack,
        Iterator,
        Union,
        Reference,
        Pointer,
        GenericParameter,
        GenericNominal,
    };

    enum class IteratorKind
    {
        Contiguous,
        Range,
    };

    enum class RangeKind
    {
        HalfOpen,
        Inclusive,
        To,
        From,
        ToInclusive,
    };

    struct Type final
    {
        TypeKind kind{TypeKind::Void};
        uint16 bitWidth{0};
        bool isSigned{false};
        bool isMutableReference{false};
        TypeKind pointeeKind{TypeKind::Void};
        uint16 pointeeBitWidth{0};
        bool pointeeIsSigned{false};
        TypeKind elementKind{TypeKind::Void};
        uint16 elementBitWidth{0};
        bool elementIsSigned{false};
        RangeKind rangeKind{RangeKind::HalfOpen};
        IteratorKind iteratorKind{IteratorKind::Contiguous};
        uint32 storageSize{0};
        uint32 storageAlignment{1};
        uint32 elementCount{0};
        uint32 elementStride{0};
        Array<Type> tupleElements;
        Array<Type> aggregateFields;
        Array<String> aggregateFieldNames;
        Array<Type> witnessParameterTypes;
        TypeKind returnKind{TypeKind::Void};
        uint16 returnBitWidth{0};
        bool returnIsSigned{false};
        Array<Type> parameterTypes;
        String symbolicName;

        static Type voidType();
        static Type intType();
        static Type uintType();
        static Type boolType();
        static Type float32x4Type();
        static Type interfaceReference();
        static Type structReference();
        static Type witnessTable(Array<Type> parameters, Type returnType, usize entryCount);
        static Type stringType();
        static Type stringSliceType();
        static Type cstringSliceType();
        static Type function(Array<Type> parameters, Type returnType);
        static Type closureType(Array<Type> parameters, Type returnType);
        static Type tuple(Array<Type> elements);
        static Type aggregate(Array<Type> fields, uint32 size, uint32 alignment);
        static Type floatType();
        static Type signedInteger(uint16 bits);
        static Type unsignedInteger(uint16 bits);
        static Type floating(uint16 bits);
        static Type reference(Type pointeeType, bool isMutable);
        static Type pointer(Type pointeeType);
        static Type range(Type elementType, RangeKind range);
        static Type buffer(Type elementType);
        static Type array(Type elementType);
        static Type slice(Type elementType);
        static Type inlineArray(
            Type elementType,
            uint32 count,
            uint32 size,
            uint32 alignment,
            uint32 stride);
        static Type variadicPack(Type elementType);
        static Type iterator(IteratorKind kind, Type elementType);
        static Type unionType(uint32 size, uint32 alignment);
        static Type genericParameter(String name);
        static Type genericNominal(String name, Array<Type> arguments);
    };

    struct Constant final
    {
        Type type{Type::voidType()};
        uint64 integer{0};
        float64 floating{0.0};
        bool boolean{false};
        String stringValue;
        Array<Constant> elements;

        static Constant integerValue(Type valueType, uint64 value)
        {
            return Constant{
                .type = valueType,
                .integer = value,
            };
        }

        static Constant floatingValue(Type valueType, float64 value)
        {
            return Constant{
                .type = valueType,
                .floating = value,
            };
        }

        static Constant booleanValue(bool value)
        {
            return Constant{
                .type = Type::boolType(),
                .boolean = value,
            };
        }

        static Constant aggregateValue(Type valueType, Array<Constant> elementValues)
        {
            return Constant{
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static Constant stringValueConstant(String value, Type valueType = Type::stringType())
        {
            return Constant{
                .type = std::move(valueType),
                .stringValue = std::move(value),
            };
        }
    };

    enum class BinaryOperator
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Equal,
        NotEqual,
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual,
        LogicalAnd,
        LogicalOr,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        LeftShift,
        RightShift,
    };

    enum class UnaryOperator
    {
        Plus,
        Negate,
        LogicalNot,
        BitwiseNot,
    };

    enum class CastPolicy
    {
        Checked,
        Truncating,
        Clamping,
    };

    enum class ValueKind
    {
        IntegerLiteral,
        FloatLiteral,
        BoolLiteral,
        CharLiteral,
        GenericValueParameter,
        StringLiteral,
        NullPointer,
        LocalLoad,
        GlobalLoad,
        FunctionReference,
        AddressOfLocal,
        AddressOfGlobal,
        AddressOfAggregateElement,
        Dereference,
        WitnessTableReference,
        PointerIsNonNull,
        PointerLoad,
        Cast,
        UnionConstruct,
        UnionTag,
        UnionPayload,
        TupleConstruct,
        TupleElement,
        AggregateConstruct,
        AggregateElement,
        AggregateSetElement,
        CompilerIntrinsicCall,
        Binary,
        Unary,
        ForeignCall,
        Call,
        IndirectCall,
    };

    struct Value final
    {
        ValueKind kind{ValueKind::IntegerLiteral};
        Type type{Type::intType()};
        uint64 integerLiteral{0};
        float64 floatLiteral{0.0};
        bool boolLiteral{false};
        String stringLiteral;
        uint32 localSlot{0};
        uint32 globalSlot{0};
        BinaryOperator binaryOperator{BinaryOperator::Add};
        UnaryOperator unaryOperator{UnaryOperator::LogicalNot};
        CastPolicy castPolicy{CastPolicy::Checked};
        uint32 operandValue{0};
        uint32 leftValue{0};
        uint32 rightValue{0};
        uint32 functionSlot{0};
        Array<uint32> arguments;
        bool hasForeignAbiSignature{false};
        bool isRuntimeCall{false};
        Type foreignAbiReturnType{Type::voidType()};
        Array<Type> foreignAbiParameterTypes;
        sema::SemanticDeclID foreignSemanticDeclId{sema::invalidSemanticDeclId};
        String foreignSymbolName;
        /// The C header declared via `@include("...")` on the parent FFI block.
        /// When non-empty, the C backend must not emit an `extern` forward declaration
        /// for this symbol — the included header already provides it.
        String foreignCIncludeHeader;
        Type foreignElementType{Type::voidType()};
        CompilerIntrinsicKind compilerIntrinsicKind{CompilerIntrinsicKind::PrimitiveCompare};
        Type aggregateObjectType{Type::voidType()};
        uint32 unionCaseTag{0};
        Array<uint32> unionPayloadOffsets;
        uint32 tupleElementIndex{0};
        uint32 aggregateFieldIndex{0};
        SourceLocation location;

        static Value integer(uint64 value, Type valueType = Type::intType())
        {
            return Value{
                .kind = ValueKind::IntegerLiteral,
                .type = valueType,
                .integerLiteral = value,
            };
        }

        static Value floating(float64 value, Type valueType = Type::floatType())
        {
            return Value{
                .kind = ValueKind::FloatLiteral,
                .type = valueType,
                .floatLiteral = value,
            };
        }

        static Value character(uint32 value)
        {
            return Value{
                .kind = ValueKind::CharLiteral,
                .type = Type::unsignedInteger(sizeof(uint32) * 8),
                .integerLiteral = value,
            };
        }

        static Value genericValueParameter(String name, Type valueType = Type::uintType())
        {
            return Value{
                .kind = ValueKind::GenericValueParameter,
                .type = valueType,
                .stringLiteral = std::move(name),
            };
        }

        static Value nullPointer(Type pointerType)
        {
            return Value{
                .kind = ValueKind::NullPointer,
                .type = std::move(pointerType),
            };
        }

        static Value boolean(bool value)
        {
            return Value{
                .kind = ValueKind::BoolLiteral,
                .type = Type::boolType(),
                .boolLiteral = value,
            };
        }

        static Value string(String value)
        {
            return Value{
                .kind = ValueKind::StringLiteral,
                .type = Type::stringType(),
                .stringLiteral = std::move(value),
            };
        }

        static Value cString(String value)
        {
            return Value{
                .kind = ValueKind::StringLiteral,
                .type = Type::cstringSliceType(),
                .stringLiteral = std::move(value),
            };
        }

        static Value localLoad(uint32 slot, Type valueType)
        {
            return Value{
                .kind = ValueKind::LocalLoad,
                .type = valueType,
                .localSlot = slot,
            };
        }

        static Value globalLoad(uint32 slot, Type valueType)
        {
            return Value{
                .kind = ValueKind::GlobalLoad,
                .type = valueType,
                .globalSlot = slot,
            };
        }

        static Value functionReference(uint32 slot)
        {
            return Value{
                .kind = ValueKind::FunctionReference,
                .type = Type::pointer(Type::voidType()),
                .functionSlot = slot,
            };
        }

        static Value addressOfLocal(uint32 slot, Type valueType)
        {
            return Value{
                .kind = ValueKind::AddressOfLocal,
                .type = valueType,
                .localSlot = slot,
            };
        }

        static Value addressOfGlobal(uint32 slot, Type valueType)
        {
            return Value{
                .kind = ValueKind::AddressOfGlobal,
                .type = valueType,
                .globalSlot = slot,
            };
        }

        static Value addressOfAggregateElement(
            uint32 aggregateAddress,
            Type valueType,
            Type objectType,
            uint32 fieldIndex)
        {
            return Value{
                .kind = ValueKind::AddressOfAggregateElement,
                .type = valueType,
                .operandValue = aggregateAddress,
                .aggregateObjectType = std::move(objectType),
                .aggregateFieldIndex = fieldIndex,
            };
        }

        static Value dereference(uint32 reference, Type resultType)
        {
            return Value{
                .kind = ValueKind::Dereference,
                .type = resultType,
                .operandValue = reference,
            };
        }

        static Value binary(BinaryOperator op, Type resultType, uint32 left, uint32 right)
        {
            return Value{
                .kind = ValueKind::Binary,
                .type = resultType,
                .binaryOperator = op,
                .leftValue = left,
                .rightValue = right,
            };
        }

        static Value unary(UnaryOperator op, Type resultType, uint32 operand)
        {
            return Value{
                .kind = ValueKind::Unary,
                .type = resultType,
                .unaryOperator = op,
                .operandValue = operand,
            };
        }

        static Value boundForeignCall(
            String symbolName,
            Type resultType,
            Array<uint32> callArguments,
            Type elementType = Type::voidType(),
            sema::SemanticDeclID semanticDeclId = sema::invalidSemanticDeclId)
        {
            return Value{
                .kind = ValueKind::ForeignCall,
                .type = resultType,
                .arguments = std::move(callArguments),
                .foreignSemanticDeclId = semanticDeclId,
                .foreignSymbolName = std::move(symbolName),
                .foreignElementType = elementType,
            };
        }

        static Value runtimeCall(
            String symbolName,
            Type resultType,
            Array<uint32> callArguments,
            Type elementType = Type::voidType(),
            sema::SemanticDeclID semanticDeclId = sema::invalidSemanticDeclId)
        {
            Value value = boundForeignCall(
                std::move(symbolName),
                resultType,
                std::move(callArguments),
                elementType,
                semanticDeclId);
            value.isRuntimeCall = true;
            return value;
        }

        static Value witnessTableReference(
            Type witnessTableType,
            Type concreteReceiverType,
            Array<uint32> functionSlots)
        {
            return Value{
                .kind = ValueKind::WitnessTableReference,
                .type = Type::pointer(std::move(witnessTableType)),
                .arguments = std::move(functionSlots),
                .aggregateObjectType = std::move(concreteReceiverType),
            };
        }

        static Value compilerIntrinsicCall(
            CompilerIntrinsicKind kind,
            Type resultType,
            Array<uint32> callArguments)
        {
            return Value{
                .kind = ValueKind::CompilerIntrinsicCall,
                .type = resultType,
                .arguments = std::move(callArguments),
                .compilerIntrinsicKind = kind,
            };
        }

        static Value foreignCall(
            String symbolName,
            Type resultType,
            Array<uint32> callArguments,
            String cIncludeHeader = {},
            bool hasAbiSignature = false,
            Type abiReturnType = Type::voidType(),
            Array<Type> abiParameterTypes = {})
        {
            return Value{
                .kind = ValueKind::ForeignCall,
                .type = std::move(resultType),
                .arguments = std::move(callArguments),
                .hasForeignAbiSignature = hasAbiSignature,
                .foreignAbiReturnType = std::move(abiReturnType),
                .foreignAbiParameterTypes = std::move(abiParameterTypes),
                .foreignSymbolName = std::move(symbolName),
                .foreignCIncludeHeader = std::move(cIncludeHeader),
            };
        }

        static Value pointerIsNonNull(uint32 pointer)
        {
            return Value{
                .kind = ValueKind::PointerIsNonNull,
                .type = Type::boolType(),
                .operandValue = pointer,
            };
        }

        static Value pointerLoad(uint32 pointer, Type resultType)
        {
            return Value{
                .kind = ValueKind::PointerLoad,
                .type = resultType,
                .operandValue = pointer,
            };
        }

        static Value cast(uint32 operand, Type resultType, CastPolicy policy = CastPolicy::Checked)
        {
            return Value{
                .kind = ValueKind::Cast,
                .type = resultType,
                .castPolicy = policy,
                .operandValue = operand,
            };
        }

        static Value makeUnion(
            Type resultType,
            uint32 tag,
            Array<uint32> payloadValues = {},
            Array<uint32> payloadOffsets = {})
        {
            return Value{
                .kind = ValueKind::UnionConstruct,
                .type = resultType,
                .arguments = std::move(payloadValues),
                .unionCaseTag = tag,
                .unionPayloadOffsets = std::move(payloadOffsets),
            };
        }

        static Value loadUnionTag(uint32 unionValue)
        {
            return Value{
                .kind = ValueKind::UnionTag,
                .type = Type::unsignedInteger(64),
                .operandValue = unionValue,
            };
        }

        static Value loadUnionPayload(uint32 unionValue, Type resultType, uint32 payloadOffset)
        {
            return Value{
                .kind = ValueKind::UnionPayload,
                .type = resultType,
                .operandValue = unionValue,
                .unionPayloadOffsets = {payloadOffset},
            };
        }

        static Value makeTuple(Type resultType, Array<uint32> elementValues)
        {
            return Value{
                .kind = ValueKind::TupleConstruct,
                .type = std::move(resultType),
                .arguments = std::move(elementValues),
            };
        }

        static Value tupleElement(uint32 tupleValue, Type resultType, uint32 elementIndex)
        {
            return Value{
                .kind = ValueKind::TupleElement,
                .type = std::move(resultType),
                .operandValue = tupleValue,
                .tupleElementIndex = elementIndex,
            };
        }

        static Value aggregateElement(uint32 aggregateValue, Type resultType, uint32 fieldIndex)
        {
            return Value{
                .kind = ValueKind::AggregateElement,
                .type = std::move(resultType),
                .operandValue = aggregateValue,
                .aggregateFieldIndex = fieldIndex,
            };
        }

        static Value makeAggregate(Type resultType, Array<uint32> fieldValues)
        {
            return Value{
                .kind = ValueKind::AggregateConstruct,
                .type = std::move(resultType),
                .arguments = std::move(fieldValues),
            };
        }

        static Value aggregateSetElement(
            uint32 aggregateValue,
            uint32 fieldValue,
            Type resultType,
            uint32 fieldIndex)
        {
            return Value{
                .kind = ValueKind::AggregateSetElement,
                .type = std::move(resultType),
                .operandValue = aggregateValue,
                .rightValue = fieldValue,
                .aggregateFieldIndex = fieldIndex,
            };
        }

        static Value call(uint32 function, Type resultType, Array<uint32> callArguments = {})
        {
            return Value{
                .kind = ValueKind::Call,
                .type = resultType,
                .functionSlot = function,
                .arguments = std::move(callArguments),
            };
        }

        static Value indirectCall(uint32 callable, Type resultType, Array<uint32> callArguments = {})
        {
            return Value{
                .kind = ValueKind::IndirectCall,
                .type = resultType,
                .operandValue = callable,
                .arguments = std::move(callArguments),
            };
        }
    };

    enum class StatementKind
    {
        StoreLocal,
        StoreGlobal,
        StoreReference,
        Evaluate,
    };

    struct Statement final
    {
        StatementKind kind{StatementKind::StoreLocal};
        uint32 localSlot{0};
        uint32 globalSlot{0};
        uint32 reference{0};
        uint32 value{0};
        SourceLocation location;

        static Statement storeLocal(uint32 slot, uint32 storedValue)
        {
            return Statement{
                .kind = StatementKind::StoreLocal,
                .localSlot = slot,
                .value = storedValue,
            };
        }

        static Statement storeGlobal(uint32 slot, uint32 storedValue)
        {
            return Statement{
                .kind = StatementKind::StoreGlobal,
                .globalSlot = slot,
                .value = storedValue,
            };
        }

        static Statement storeReference(uint32 storedReference, uint32 storedValue)
        {
            return Statement{
                .kind = StatementKind::StoreReference,
                .reference = storedReference,
                .value = storedValue,
            };
        }

        static Statement evaluate(uint32 evaluatedValue)
        {
            return Statement{
                .kind = StatementKind::Evaluate,
                .value = evaluatedValue,
            };
        }
    };

    enum class TerminatorKind
    {
        Return,
        Branch,
        ConditionalBranch,
        Unreachable,
    };

    struct Terminator final
    {
        TerminatorKind kind{TerminatorKind::Return};
        Optional<uint32> returnValue;
        uint32 targetBlock{0};
        uint32 conditionValue{0};
        uint32 trueBlock{0};
        uint32 falseBlock{0};
        SourceLocation location;

        static Terminator returnValueTerminator(uint32 value)
        {
            return Terminator{
                .kind = TerminatorKind::Return,
                .returnValue = value,
            };
        }

        static Terminator returnTerminator()
        {
            return Terminator{
                .kind = TerminatorKind::Return,
            };
        }

        static Terminator branch(uint32 target)
        {
            return Terminator{
                .kind = TerminatorKind::Branch,
                .targetBlock = target,
            };
        }

        static Terminator conditionalBranch(uint32 condition, uint32 trueTarget, uint32 falseTarget)
        {
            return Terminator{
                .kind = TerminatorKind::ConditionalBranch,
                .conditionValue = condition,
                .trueBlock = trueTarget,
                .falseBlock = falseTarget,
            };
        }

        static Terminator unreachable()
        {
            return Terminator{
                .kind = TerminatorKind::Unreachable,
            };
        }
    };

    String describeType(Type const& type);
}
