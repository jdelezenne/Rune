#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::llvm
{
    struct EmissionOptions final
    {
        String outputPath;
    };

    struct NativeEmissionOptions final
    {
        String objectPath;
        String executablePath;
        Array<String> dependencyObjectPaths;
        Array<String> linkLibraries;
        Array<String> linkFrameworks;
        Array<String> linkDirectories;
        double* objectEmissionMs{nullptr};
        double* linkingMs{nullptr};
    };

    Result<bool> emitObjectFile(
        ril::Module const& module,
        String const& objectPath);

    Result<String> emitModuleToString(ril::Module const& module);

    Result<bool> emitModule(
        ril::Module const& module,
        EmissionOptions const& options);

    Result<bool> emitNativeExecutable(
        ril::Module const& module,
        NativeEmissionOptions const& options);
}
