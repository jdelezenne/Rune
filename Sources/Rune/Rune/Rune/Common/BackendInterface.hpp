#pragma once

#include "Rune/Common/VerboseFlags.hpp"
#include "Rune/Core/Array.hpp"
#include "Rune/Core/Platform.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/RIL/Module.hpp"

#include <filesystem>

namespace rune
{
    struct ModuleBuildRequest;

    struct BackendEmitContext
    {
        ModuleBuildRequest const& request;
        std::filesystem::path artifactDirectory;
        ril::Module const* rilModule{nullptr};
    };

    struct BackendExecuteContext
    {
        String modulePath;
        String moduleName;
        String artifactDirectory;
        /// Explicit .runemodule paths of all preloaded dependency artifacts.
        Array<String> dependencyPaths;
        /// Directories containing .runemodule dependency interfaces.
        Array<String> modulePaths;
        Array<String> linkLibraries;
        Array<String> linkDirectories;
        Array<String> programArgs;
        VerboseFlag verboseFlags{VerboseFlag::None};
        bool profile{false};
        Platform::HostPlatform targetPlatform{Platform::getHostPlatform()};
    };

    class BackendInterface
    {
    public:
        virtual ~BackendInterface() = default;
        virtual bool isNative() const = 0;
        virtual Result<bool> emit(BackendEmitContext& ctx) = 0;
        virtual int execute(BackendExecuteContext const& ctx) = 0;
    };
}
