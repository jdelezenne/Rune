#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::backend
{
    struct ArtifactBuildOptions final
    {
        String moduleName;
        bool emitDebugInfo{false};
    };

    struct EmissionOptions final
    {
        String outputDirectory;
        String moduleName;
        bool emitDebugInfo{false};
    };

    Result<artifact::Module> buildModuleArtifact(
        ril::Module const& module,
        ArtifactBuildOptions const& options);

    Result<bool> emitModule(ril::Module const& module, EmissionOptions const& options);
}
