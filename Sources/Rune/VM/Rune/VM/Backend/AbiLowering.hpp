#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::backend
{
    Result<artifact::AbiIndex> lowerAbiRecord(
        artifact::Module& module,
        ril::Module const& rilModule,
        String const& symbolName);
}
