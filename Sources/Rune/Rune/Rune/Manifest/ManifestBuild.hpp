#pragma once

#include "Rune/Common/CompilerCommon.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Parser/AST.hpp"

namespace rune
{
    struct ModuleArtifactPaths final
    {
        String modulePath;
        String artifactDirectory;
    };

    ModuleArtifactPaths resolveModuleArtifactPaths(
        String const& moduleName,
        String const& outputDirectory);

    struct ModuleBuildRequest final
    {
        Array<String> sourceFiles;
        String moduleName;
        String outputPath;
        /// Explicit dependency paths: each must be a .runemodule or .runemanifest path.
        Array<String> dependencyPaths;
        CompilerOptions compilerOptions;
        Optional<ModuleManifest> manifest;
        BackendKind backend{BackendKind::None};
        bool quiet{false};
        /// Populated by buildModuleFromRequest: .runemodule paths of all compiled dep nodes.
        Array<String> builtDependencyPaths;
        /// Populated by buildModuleFromRequest: backend artifacts required to link the root.
        Array<String> builtDependencyObjectPaths;
    };

    Result<ModuleBuildRequest> moduleBuildRequestFromManifest(
        ModuleManifest const& manifest,
        CompilerOptions const& cliOptions);

    Result<String> resolveSourceModuleName(
        Array<String> const& sourceFiles,
        String const& moduleName);

    Result<bool> prepareExecutableEntryPoint(
        parser::Program& program,
        ModuleDeclKind moduleKind);

    bool buildModuleFromRequest(ModuleBuildRequest& request);

    bool buildModuleFromManifest(
        String const& manifestPath,
        CompilerOptions const& cliOptions,
        ModuleManifestLoadOptions const& loadOptions = {},
        ModuleManifestBuildOverrides const& overrides = {});
}
