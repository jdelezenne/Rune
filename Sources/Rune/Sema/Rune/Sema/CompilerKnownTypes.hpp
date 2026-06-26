#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    /// Primitive type spellings known to the compiler.
    enum class PrimitiveTypeKind : uint8
    {
        None,
        Int8,
        Int16,
        Int32,
        Int64,
        Int,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        UInt,
        Float32,
        Float64,
        Bool,
        Char,
        Byte,
        Void,
    };

    /// Standard module declarations that the compiler knows for shorthand/literal rules.
    enum class KnownStandardTypeKind : uint8
    {
        None,
        NilLiteral,
        BooleanLiteral,
        CharacterLiteral,
        IntegerLiteral,
        FloatingPointLiteral,
        StringLiteral,
        ArrayLiteral,
        DictionaryLiteral,
        String,
        CString,
        CStringSlice,
        StringSlice,
        Option,
        Result,
        Buffer,
        Array,
        Slice,
        InlineArray,
        HashMap,
        HashSet,
        SortedMap,
        SortedSet,
        Range,
        RangeInclusive,
        RangeTo,
        RangeFrom,
        RangeToInclusive,
        Float32x4,
    };

    void seedCompilerKnownTypes(ASTContext& context);

    Optional<TypeID> lookupPrimitiveType(ASTContext const& context, StringView name);
    Optional<TypeID> lookupPrimitiveType(ASTContext const& context, IdentifierID nameId);
    Optional<TypeID> lookupKnownStandardType(ASTContext const& context, StringView name);
    Optional<TypeID> lookupKnownStandardType(ASTContext const& context, IdentifierID nameId);
    Optional<TypeID> lookupCompilerKnownType(ASTContext const& context, StringView name);
    Optional<TypeID> lookupCompilerKnownType(ASTContext const& context, IdentifierID nameId);
    Optional<TypeID> typeForInterfaceConstant(ASTContext& context, InterfaceConstantRecord const& constant);
    Optional<TypeID> typeForInterfaceScopeConstant(
        ASTContext& context,
        Module const& module,
        InterfaceConstantRecord const& constant);
    Optional<EnumCaseValue> enumCaseValueFromInterfaceScopeConstant(
        ASTContext& context,
        Module const& module,
        TypeID nominalTypeId,
        InterfaceConstantRecord const& constant);

    PrimitiveTypeKind primitiveTypeKind(ASTContext const& context, TypeID typeId);
    void registerDistinctPrimitiveKind(ASTContext& context, TypeID typeId, PrimitiveTypeKind kind);
    KnownStandardTypeKind knownStandardTypeKind(ASTContext const& context, TypeID typeId);

    bool isPrimitiveType(ASTContext const& context, TypeID typeId);
    bool isKnownStandardType(ASTContext const& context, TypeID typeId);
    bool isRangeTypeKind(KnownStandardTypeKind kind);
    bool isRangeType(ASTContext const& context, TypeID typeId);

    bool isIntegerPrimitive(PrimitiveTypeKind kind);
    bool isUnsignedIntegerPrimitive(PrimitiveTypeKind kind);
    bool isFloatPrimitive(PrimitiveTypeKind kind);
    bool isNumericPrimitive(PrimitiveTypeKind kind);
    Optional<uint32> primitiveTypeSize(PrimitiveTypeKind kind);
    Optional<uint32> primitiveTypeAlignment(PrimitiveTypeKind kind);

    StringView primitiveTypeName(PrimitiveTypeKind kind);
    StringView knownStandardTypeName(KnownStandardTypeKind kind);
    StringView knownStandardCBuiltinStorageName(KnownStandardTypeKind kind);
}
