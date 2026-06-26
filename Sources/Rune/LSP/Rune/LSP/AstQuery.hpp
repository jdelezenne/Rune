#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/LSP/AnalysisService.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/SymbolTable.hpp"

namespace rune::lsp
{
    struct AnalysisSnapshot final
    {
        ASTContext& context;
        sema::SymbolTable& symbols;
        parser::Program const& program;
        String activeFilePath;
    };

    struct PositionTarget final
    {
        BoundDecl const* boundDecl = nullptr;
        Optional<sema::ResolvedSymbol> resolved;
        String name;
        SourceLocation tokenLocation;
        SourceLocation definitionLocation;
        TypeID typeId = 0;
    };

    Optional<PositionTarget> findPositionTarget(
        AnalysisSnapshot const& snapshot,
        uint32 runeLine,
        uint32 runeColumn);

    Array<SourceLocation> findReferenceLocations(
        AnalysisSnapshot const& snapshot,
        PositionTarget const& target);

    struct MemberAccessSite final
    {
        parser::Expression const* receiverObject = nullptr;
        parser::Expression const* memberExpression = nullptr;
        bool leadingDot = false;
        DeclContext const* fromContext = nullptr;
    };

    Optional<MemberAccessSite> findMemberAccessSite(
        AnalysisSnapshot const& snapshot,
        uint32 runeLine,
        uint32 runeColumn);

    /// Returns the innermost declaration context containing @p runeLine / @p runeColumn.
    DeclContext const& findDeclContextAtPosition(
        AnalysisSnapshot const& snapshot,
        uint32 runeLine,
        uint32 runeColumn);
}
