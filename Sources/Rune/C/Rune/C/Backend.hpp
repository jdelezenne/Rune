#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::c
{
    struct EmissionOptions final
    {
        String outputDirectory;
        String moduleName;
    };

    Result<bool> emitModule(
        ril::Module const& module,
        EmissionOptions const& options);
}
