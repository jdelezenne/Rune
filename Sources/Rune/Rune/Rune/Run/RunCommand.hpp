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
        /// Native libraries to link (source builds only; manifests use "linkLibraries").
        Array<String> linkLibraries;
        /// C headers the C backend must #include for FFI symbols (source builds only;
        /// manifests use "includeHeaders").
        Array<String> includeHeaders;
        Array<String> programArgs;
        VerboseFlag verboseFlags{VerboseFlag::None};
        BackendKind backend{BackendKind::VM};
        bool printStatistics = false;
        bool printStatisticsDetail = false;
        bool profile = false;
        bool allowUnsafe = false;
    };

    int runCommand(RunCommandOptions const& options);
}
