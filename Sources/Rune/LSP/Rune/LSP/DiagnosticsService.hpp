#pragma once

#include "Rune/LSP/AnalysisService.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json buildDiagnostics(String const& uri, AnalysisResult const& analysis);
}
