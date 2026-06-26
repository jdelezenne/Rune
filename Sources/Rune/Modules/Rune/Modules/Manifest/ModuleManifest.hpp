#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Platform.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Version.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    inline constexpr StringView sourceExtension = ".rune";
    inline constexpr StringView moduleExtension = ".runemodule";
    inline constexpr StringView moduleManifestExtension = ".runemanifest";
    inline constexpr Version moduleManifestVersion = {1, 0, 0};

    /// One dependency: a path to a `.runemodule` or `.runemanifest` file.
    /// Relative paths are resolved at load time relative to the manifest directory.
    struct ModuleManifestDependency final
    {
        String path;
    };

    struct ModuleManifestResource final
    {
        /// Absolute source path resolved relative to the declaring manifest.
        String path;
        /// Relative path below the artifact's Resources directory.
        String destination;
    };

    /// On-disk serialization format for a `.runemanifest` file.
    enum class ModuleManifestFormat
    {
        Json,
    };

    struct ModuleManifestLoadOptions final
    {
        Optional<Platform::HostPlatform> platformOverride;
    };

    struct ModuleManifestBuildOverrides final
    {
        Optional<String> outputPath;
    };

    /// Authoritative module build description.
    struct ModuleManifest final
    {
        String manifestPath;
        String manifestDirectory;
        Platform::HostPlatform targetPlatform{Platform::getHostPlatform()};
        Version version{moduleManifestVersion};
        String name;
        ModuleDeclKind kind{ModuleDeclKind::Library};
        Array<String> sources;
        Array<ModuleManifestResource> resources;
        Array<ModuleManifestDependency> dependencies;
        Array<String> includeHeaders;
        Array<String> linkLibraries;
        Array<String> linkFrameworks;
        Array<String> linkDirectories;
        String output;
        String format{"binary"};
        ModuleBuildConfiguration configuration{ModuleBuildConfiguration::Release};
        /// Enables module-level unsafe features such as direct @symbol FFI declarations.
        bool unsafe{false};
        Array<String> alwaysImported;
    };

    bool isModuleManifestPath(StringView path);

    /// Detects the serialization format of a `.runemanifest` file from its path and content.
    Optional<ModuleManifestFormat> moduleManifestFormatFromPath(String const& manifestPath);

    /// Load and validate a `.runemanifest` file. Relative paths resolve against the manifest directory.
    Result<ModuleManifest> loadModuleManifest(
        String const& manifestPath,
        ModuleManifestLoadOptions const& options = {});

    void applyModuleManifestBuildOverrides(ModuleManifest& manifest, ModuleManifestBuildOverrides const& overrides);

    /// Write a manifest to disk. Paths in `manifest` are stored relative to the manifest directory when possible.
    Result<Void> saveModuleManifest(
        ModuleManifest const& manifest,
        String const& manifestPath,
        ModuleManifestFormat format = ModuleManifestFormat::Json);

    /// Manifest mode: rejects unknown CLI options; the manifest file is authoritative.
    Optional<String> validateModuleCommandManifestCli(int startIndex, int argc, char* argv[]);

    /// Parse known manifest-mode CLI overrides after validation succeeds.
    ModuleManifestBuildOverrides parseModuleManifestBuildOverrides(int startIndex, int argc, char* argv[]);

    Optional<Platform::HostPlatform> parseTargetPlatformOption(int startIndex, int argc, char* argv[]);

    /// Returns the single `.runemanifest` positional argument, if present.
    Optional<String> findModuleManifestArgument(int startIndex, int argc, char* argv[]);

    /// True when a `.runemanifest` positional is present and no `.rune` positionals appear.
    bool moduleCommandUsesManifest(int startIndex, int argc, char* argv[]);

    /// Default artifact path when manifest `output` is empty: `<manifest-dir>/<name>.runemodule`.
    String defaultModuleArtifactOutputPath(ModuleManifest const& manifest);
}
