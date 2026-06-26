#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

namespace rune::vm::interpreter
{
    struct FrameLayout final
    {
        uint32 frameBytes{0};
        uint32 frameAlignment{1};
        uint32 valueSlotCount{0};
    };

    struct Frame final
    {
        FrameLayout layout;
        Array<uint8> storage;
        Array<Value> values;
    };

    struct RuntimeFrame final
    {
        uint32 function{0xFFFF'FFFFu};
        uint32 instruction{0xFFFF'FFFFu};
        Array<uint8>* storage{nullptr};
        Array<Value>* values{nullptr};
    };
}
