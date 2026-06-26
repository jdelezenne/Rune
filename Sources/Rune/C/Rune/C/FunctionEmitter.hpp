#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::c
{
    struct FunctionEmitter final
    {
        ril::Module const& module;
        ril::Function const& function;
        usize functionIndex{0};

        Result<String> emitDeclaration() const;
        Result<String> emitDefinition() const;
    };
}
