#pragma once

#include "Rune/Formats/Module/Format.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/BuilderContext.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::package
{
    struct PackageBuildOptions final
    {
        ASTContext context;
        Optional<ModuleManifest> manifest;
        String outputPath;
        bool verbose{false};
        ModuleBuildConfiguration configuration{ModuleBuildConfiguration::Release};
        /// When set (typically after `ril::buildModuleInto`), serialized
        /// implementation bodies reuse this builder's dependency cache.
        ril::ModuleBuilder* serializedBodyBuilder{nullptr};
    };

    struct PackageDebugDeclRecord final
    {
        DeclID declId{0};
        uint32 sourceFileIndex{0};
        uint32 line{0};
        uint32 column{0};
    };

    /// Introspection section records are shared with the context-bound module
    /// representation (`rune::IntrospectionTypeRecord` in `Sema/ASTContext.hpp`).

    struct Module final
    {
        String name;
        String artifactPath;
        Version formatVersion;
        ModuleDeclKind kind{ModuleDeclKind::Library};
        ModuleBuildConfiguration configuration{ModuleBuildConfiguration::Release};
        Array<String> dependencyModules;
        Array<String> dependencyVersions;
        Array<String> includeHeaders;
        Array<String> linkLibraries;
        Array<String> linkFrameworks;
        Array<String> linkDirectories;
        Array<String> identifiers;
        Array<InterfaceDeclRecord> interfaceDecls;
        Array<InterfaceConstantRecord> interfaceConstants;
        Array<InterfaceStructStorageLayoutRecord> interfaceStructStorageLayouts;
        Array<InterfaceConformanceRecord> interfaceConformances;
        Array<InterfaceWitnessImportRecord> interfaceWitnesses;
        Array<InterfaceInheritanceRecord> interfaceInheritance;
        Array<SemanticType> interfaceTypes;
        Array<ForeignBinding> foreignBindings;
        Array<SerializedImplementationBodyRecord> serializedImplementationBodies;
        Array<uint8> serializedImplementationPayloads;
        Array<ModuleSectionRecord> sections;
        Array<String> debugSourceFiles;
        Array<PackageDebugDeclRecord> debugDecls;
        Array<IntrospectionTypeRecord> introspectionTypes;

        StringView lookupIdentifier(IdentifierID id) const
        {
            if (id >= identifiers.size())
            {
                return {};
            }

            return identifiers[id];
        }
    };
}
