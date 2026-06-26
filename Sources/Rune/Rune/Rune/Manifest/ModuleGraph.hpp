#pragma once

#include "Rune/Common/CompilerCommon.hpp"
#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Platform.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    /// Pre-computed output paths for one module node.
    ///
    /// The interface path is always set. Backend artifact paths are set only
    /// when the selected backend is the one that produces that artifact type.
    ///
    /// Backend artifacts live in module-scoped subdirectories alongside the
    /// semantic interface:
    ///
    ///   <outputDir>/
    ///     Name.runemodule        <- interfacePath
    ///     vm/
    ///       Name.runebc          <- vmPath
    ///     c/
    ///       Name.c               <- cSourcePath
    ///       Name.h               <- cHeaderPath
    ///     llvm/
    ///       Name.o               <- llvmObjectPath
    struct ArtifactPlan final
    {
        String interfacePath;
        Optional<String> vmPath;
        Optional<String> cSourcePath;
        Optional<String> cHeaderPath;
        Optional<String> llvmObjectPath;
    };

    /// One node in the module dependency graph.
    struct ModuleGraphNode final
    {
        /// Module name as declared in the manifest.
        String name;
        /// Absolute canonical path to the source manifest. Empty when isPrebuilt.
        String canonicalManifestPath;
        /// Absolute path to the prebuilt .runemodule artifact. Set when isPrebuilt.
        Optional<String> prebuiltArtifactPath;
        ModuleDeclKind kind{ModuleDeclKind::Library};
        Platform::HostPlatform targetPlatform{Platform::getHostPlatform()};
        BackendKind backend{BackendKind::None};
        /// Indices into ModuleGraph::nodes for direct dependencies.
        /// Dependencies always appear earlier in the array (topological order).
        Array<size_t> dependencyIndices;
        /// Pre-computed artifact paths for this node.
        ArtifactPlan plan;
        /// True when this node represents a prebuilt artifact with no source manifest.
        bool isPrebuilt{false};
    };

    /// Fully resolved, topologically ordered module dependency graph.
    /// nodes[0..N-2] are dependencies; nodes[N-1] is the root module being built.
    struct ModuleGraph final
    {
        Array<ModuleGraphNode> nodes;
        /// Indices into nodes for dependencies declared directly by the root build request.
        Array<size_t> rootDependencyIndices;
    };

    /// Compute the ArtifactPlan for a module given its output directory and selected backend.
    ArtifactPlan planModuleArtifacts(
        String const& moduleName,
        String const& outputDirectory,
        BackendKind backend);
}
