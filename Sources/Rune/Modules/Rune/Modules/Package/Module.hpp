#pragma once

#include "Rune/Formats/Module/Format.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Parser/AST.hpp"
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
    };

    struct PackageDebugDeclRecord final
    {
        DeclID declId{0};
        uint32 sourceFileIndex{0};
        uint32 line{0};
        uint32 column{0};
    };

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
        Array<GenericImplementationBodyRecord> genericImplementationBodies;
        Array<uint8> genericImplementationPayloads;
        Array<ModuleSectionRecord> sections;
        Array<String> debugSourceFiles;
        Array<PackageDebugDeclRecord> debugDecls;

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
