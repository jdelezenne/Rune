#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"

namespace rune::lsp
{
    int runLanguageServer(Array<String> const& moduleSearchPaths = {});
}
