#pragma once

#include "Rune/LSP/AstQuery.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json buildSignatureHelp(
        AnalysisSnapshot const& snapshot,
        StringView documentText,
        uint32 runeLine,
        uint32 runeColumn);
}
