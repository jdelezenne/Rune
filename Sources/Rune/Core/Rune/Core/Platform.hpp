#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/StringView.hpp"

#include <filesystem>
namespace rune
{
    namespace Platform
    {
        enum class HostPlatform : uint8
        {
            Darwin,
            Linux,
            Windows,
            Wasm,
        };

        HostPlatform getHostPlatform();

        StringView hostPlatformName(HostPlatform platform);

        Optional<HostPlatform> parseHostPlatform(StringView name);

        /// CLI override wins over manifest default, then host platform.
        HostPlatform resolveTargetPlatform(
            Optional<HostPlatform> cliOverride,
            Optional<HostPlatform> manifestPlatform = std::nullopt);

        /// Absolute path to the current process executable.
        std::filesystem::path getExecutablePath();

        /// Directory containing the current process executable.
        std::filesystem::path getExecutableDirectory();
    }
}
