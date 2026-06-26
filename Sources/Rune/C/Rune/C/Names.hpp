#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::c
{
    namespace names
    {
        inline constexpr StringView privateAggregatePrefix = "__rune_";
        inline constexpr StringView unionTypePrefix = "__rune_union_";
        inline constexpr StringView entryFunctionSymbol = "__rune_entry";
        inline constexpr StringView functionSymbolPrefix = "__rune_fn_";
        inline constexpr StringView globalSymbolPrefix = "__rune_global_";
        inline constexpr StringView witnessTableSymbolPrefix = "__rune_witness_";
        inline constexpr StringView witnessAdapterSymbolPrefix = "__rune_witness_adapter_";
    }

    String cPrivateIdentifier(StringView name);
    String moduleFileStem(StringView moduleName);
    String functionSymbol(ril::Function const& function, usize functionIndex);
    String globalSymbol(ril::Global const& global, usize globalIndex);
    String staticDataSymbol(ril::StaticData const& data, usize dataIndex);
    String localSymbol(usize localIndex);
    String valueSymbol(usize valueIndex);
    String fieldSymbol(usize fieldIndex);
    String blockLabel(usize blockIndex);
}
