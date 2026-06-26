#pragma once

#include "Rune/Lexer/Token.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/LiteralTyping.hpp"
#include "Rune/Sema/PrimitiveConversion.hpp"

namespace rune::sema
{
    struct TypeBindContext;

    enum class ConversionKind : uint8
    {
        Exact,
        Implicit,
        ExplicitInit,
        Incompatible,
    };

    ConversionKind conversionKind(ASTContext const& context, TypeID from, TypeID to);
    bool isMutableByteReferenceType(ASTContext const& context, TypeID typeId);
    bool isImmutableByteReferenceType(ASTContext const& context, TypeID typeId);
    bool isReferenceType(ASTContext const& context, TypeID typeId);
    bool isMachineAddressIntegerType(ASTContext const& context, TypeID typeId);
    /// Maps `Int`/`UInt` to the platform fixed-width integer type (`Int32`/`Int64` or `UInt32`/`UInt64`).
    Optional<TypeID> machineWidthFixedIntegerType(ASTContext const& context, TypeID typeId);
    bool isByteAddressConversion(ASTContext const& context, TypeID from, TypeID to);

    bool trySetByteAddressPrimitiveConversion(
        ASTContext& context,
        parser::Expression const& expression,
        TypeID argumentType,
        TypeID resultType,
        HashMap<parser::Expression const*, PrimitiveConversion>& primitiveConversions);

    /// Unwraps `&Collection` (and similar refs) to the collection type for subscript lookup.
    TypeID collectionReceiverType(TypeBindContext const& bindContext, TypeID typeId);

    bool isPrimitiveNumericConversion(ASTContext const& context, TypeID from, TypeID to);
    bool isAlwaysSafePrimitiveNumericConversion(
        ASTContext const& context,
        TypeID from,
        TypeID to);
    Optional<PrimitiveConversionPolicy> primitiveConversionStaticHelperPolicy(
        ASTContext& context,
        IdentifierID helperNameId);

    bool checkConversion(
        ASTContext const& context,
        TypeID from,
        TypeID to,
        parser::ASTNode const& errorNode,
        DiagnosticEngine& diagnostics,
        bool allowExplicitInit = false,
        Optional<LiteralMetadata> literalMetadata = std::nullopt);

    bool canConvert(
        ASTContext const& context,
        TypeID from,
        TypeID to,
        bool allowExplicitInit = false,
        Optional<LiteralMetadata> literalMetadata = std::nullopt);

    bool integerLiteralFits(lexer::Token const& token, PrimitiveTypeKind targetKind);

    bool isExplicitNumericConversion(ASTContext const& context, TypeID target, TypeID argument);

    bool isExplicitConversion(ASTContext const& context, TypeID target, TypeID argument);

}
