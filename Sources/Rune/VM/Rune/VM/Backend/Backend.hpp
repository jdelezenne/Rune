#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::vm::backend
{
    struct EmissionOptions final
    {
        String outputDirectory;
        String moduleName;
        bool emitDebugInfo{false};
    };

    Result<bool> emitModule(ril::Module const& module, EmissionOptions const& options);
}
