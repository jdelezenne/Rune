#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::artifact
{
    Result<bool> writeModuleArtifact(Module const& module, String const& path);
    Result<Array<uint8>> encodeModuleArtifact(Module const& module);
}
