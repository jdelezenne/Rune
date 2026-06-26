#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/LSP/AstQuery.hpp"
#include "Rune/LSP/JsonRpcTransport.hpp"

namespace rune::lsp
{
    Optional<Json> buildHover(AnalysisSnapshot const& snapshot, PositionTarget const& target);
}
