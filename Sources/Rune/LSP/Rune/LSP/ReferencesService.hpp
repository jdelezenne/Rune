#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Json buildReferences(Array<SourceLocation> const& locations, String const& fileUri);
}
