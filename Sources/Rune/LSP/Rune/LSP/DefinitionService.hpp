#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/LSP/AstQuery.hpp"

namespace rune::lsp
{
    Optional<SourceLocation> findDefinition(
        AnalysisSnapshot const& snapshot,
        uint32 runeLine,
        uint32 runeColumn);
}
