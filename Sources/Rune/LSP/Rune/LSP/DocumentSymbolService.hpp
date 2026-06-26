#pragma once

#include "Rune/LSP/AstQuery.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json buildDocumentSymbols(AnalysisSnapshot const& snapshot, String const& documentUri);
}
