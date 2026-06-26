#pragma once

#include "Rune/Core/StringView.hpp"

namespace rune::llvm::names
{
    inline constexpr StringView entryBlock = "entry";
    inline constexpr StringView mainFunction = "main";
    inline constexpr StringView entryFunctionSymbol = "__rune_entry";
    inline constexpr StringView witnessTableSymbolPrefix = "__rune_witness_";
    inline constexpr StringView witnessAdapterSymbolPrefix = "__rune_witness_adapter_";
}
