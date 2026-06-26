#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"

namespace rune
{
    struct CBackendRunOptions final
    {
        String sourceDirectory;
        String executablePath;
        Array<String> linkLibraries;
        Array<String> linkDirectories;
        Array<String> programArgs;
    };

    int runCBackendExecutable(CBackendRunOptions const& options);
}
