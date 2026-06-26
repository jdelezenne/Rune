#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Lexer/Token.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/CompilerKnownTypes.hpp"
#include "Rune/Sema/LiteralMetadata.hpp"
#include "Rune/Sema/SemanticTypePrinting.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune::sema
{
    bool isLiteralType(ASTContext const& context, TypeID typeId);
    LiteralCategory literalCategoryOf(ASTContext const& context, TypeID typeId);
    TypeID literalCategoryType(ASTContext& context, LiteralCategory category);

    LiteralMetadata metadataForIntegerToken(lexer::Token const& token);
    LiteralMetadata metadataForFloatToken(lexer::Token const& token);
    LiteralMetadata metadataForNegatedInteger(LiteralMetadata const& operand, parser::ASTNode const& errorNode, DiagnosticEngine& diagnostics);

    StringView describeLiteralCategory(LiteralCategory category);
    StringView describeLiteralType(ASTContext const& context, TypeID typeId, Optional<LiteralMetadata> metadata);

    Optional<TypeID> defaultTypeForLiteral(ASTContext const& context, LiteralMetadata const& metadata);

    bool literalMetadataCanApplyToTarget(
        ASTContext const& context,
        LiteralMetadata const& metadata,
        TypeID targetType);

    bool literalFitsType(
        ASTContext const& context,
        LiteralMetadata const& metadata,
        TypeID targetType,
        parser::ASTNode const& errorNode,
        DiagnosticEngine& diagnostics);

    bool canLiteralFitType(
        ASTContext const& context,
        LiteralMetadata const& metadata,
        TypeID targetType);

    Optional<TypeID> resolveLiteralToType(
        ASTContext const& context,
        LiteralMetadata const& metadata,
        TypeID targetType,
        parser::ASTNode const& errorNode,
        DiagnosticEngine& diagnostics);

    Optional<TypeID> materializeLiteralType(
        ASTContext const& context,
        LiteralMetadata const& metadata,
        Optional<TypeID> contextualType,
        parser::ASTNode const& errorNode,
        DiagnosticEngine& diagnostics);

    bool checkLiteralConversion(
        ASTContext const& context,
        TypeID from,
        TypeID to,
        Optional<LiteralMetadata> metadata,
        parser::ASTNode const& errorNode,
        DiagnosticEngine& diagnostics,
        bool allowExplicitInit = false);
}
