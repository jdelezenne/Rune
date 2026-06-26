#pragma once

#include "Rune/LSP/AstQuery.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json buildCompletionItems(
        AnalysisSnapshot const& snapshot,
        String const& documentText,
        uint32 runeLine,
        uint32 runeColumn);
}
