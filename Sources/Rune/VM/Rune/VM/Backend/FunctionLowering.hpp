#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::backend
{
    struct FunctionLoweringContext final
    {
        artifact::Module& artifactModule;
        ril::Module const& rilModule;
    };

    Result<artifact::FunctionIndex> lowerFunction(
        FunctionLoweringContext& context,
        ril::Function const& function);
}
