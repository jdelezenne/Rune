#pragma once

#include "Rune/Core/SourceLocation.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json sourceLocationToRange(SourceLocation const& location);
    Json sourceLocationToLspLocation(SourceLocation const& location);
}
