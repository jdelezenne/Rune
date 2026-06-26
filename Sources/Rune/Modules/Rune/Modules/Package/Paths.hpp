#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

#include <filesystem>
namespace rune
{
    inline constexpr StringView artifactDirectoryName = "modules";

    namespace PackagePaths
    {
        /// Override artifact search directory (used by tests).
        void setArtifactDirectoryOverride(std::filesystem::path directory);

        /// Directory containing built-in `.runemodule` artifacts next to the compiler.
        std::filesystem::path artifactDirectory();

        /// Resolve a module name to a `.runemodule` path using search paths.
        Optional<std::filesystem::path> findPackage(
            StringView moduleName,
            Array<String> const& searchPaths);

        /// Returns the vm/ subdirectory path for the .runebc artifact that corresponds
        /// to a given .runemodule interface artifact.
        /// Example: /out/Standard.runemodule -> /out/vm/Standard.runebc
        std::filesystem::path bytecodeArtifactPathForModuleArtifact(std::filesystem::path const& moduleArtifactPath);
    }
}
