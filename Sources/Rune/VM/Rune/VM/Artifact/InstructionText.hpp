#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/VM/Artifact/Instruction.hpp"

namespace rune::vm::artifact
{
    struct InstructionTextOptions final
    {
        StringView invalidIndexText{"invalid"};
        bool prefixValueSlots{false};
        bool includeOperatorFields{true};
    };

    String formatIndex(uint32 value, StringView invalidText);
    String formatValueSlot(ValueSlot slot, InstructionTextOptions const& options);
    String formatInstruction(Instruction const& instruction, InstructionTextOptions const& options = {});
}
