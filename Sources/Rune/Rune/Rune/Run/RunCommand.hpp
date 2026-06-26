#pragma once

#include "Rune/Common/CompilerCommon.hpp"
#include "Rune/Common/VerboseFlags.hpp"
#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"

namespace rune
{
    struct RunCommandOptions final
    {
        Array<String> sourceFiles;
        String manifestPath;
        String modulePath;
        Array<String> dependencyPaths;
        Array<String> modulePaths;
        Array<String> programArgs;
        VerboseFlag verboseFlags{VerboseFlag::None};
        BackendKind backend{BackendKind::VM};
        bool printStatistics = false;
    };

    int runCommand(RunCommandOptions const& options);
}
