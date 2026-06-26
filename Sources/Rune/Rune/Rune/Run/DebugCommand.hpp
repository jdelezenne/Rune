#pragma once

#include "Rune/Common/CompilerCommon.hpp"

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"

namespace rune
{
    struct DebugCommandOptions final
    {
        Array<String> sourceFiles;
        String manifestPath;
        String modulePath;
        Array<String> dependencyPaths;
        Array<String> modulePaths;
        BackendKind backend{BackendKind::VM};
    };

    int debugCommand(DebugCommandOptions const& options);
}
