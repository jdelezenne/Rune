#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::artifact
{
    Result<Module> readModuleArtifact(String const& path);
    Result<Module> decodeModuleArtifact(Array<uint8> const& bytes, String const& artifactPath = {});
}
