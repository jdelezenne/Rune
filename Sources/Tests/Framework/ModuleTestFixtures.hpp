#pragma once

#include "Testing/ModulePathsTestSetup.hpp"

#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Modules/Package/ImportBinding.hpp"
#include "Rune/Modules/Package/Module.hpp"
#include "Rune/Modules/Package/Paths.hpp"
#include "Rune/Modules/Package/Reader.hpp"
#include "Rune/Modules/Package/Writer.hpp"
#include "Rune/Parser/Pipeline.hpp"
#include "Rune/Sema/CompilationSession.hpp"
#include "Rune/Sema/Sema.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

namespace rune::test
{
    /// RAII workspace under the system temp directory for module round-trip tests.
    struct TempDirectory final
    {
        std::filesystem::path path;

        explicit TempDirectory(std::string_view tag)
        {
            path = std::filesystem::temp_directory_path() / tag;
            std::filesystem::remove_all(path);
            std::filesystem::create_directories(path);
        }

        ~TempDirectory()
        {
            std::filesystem::remove_all(path);
        }
    };

    inline void writeSourceFile(std::filesystem::path const& path, std::string_view contents)
    {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        REQUIRE(output);
        output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
    }

    inline Result<bool> analyzeAndWritePackage(
        package::PackageBuildOptions& buildOptions,
        parser::Program const& program)
    {
        if (!sema::prepareModule(buildOptions.context, program))
        {
            return SourceError("Failed to prepare module for semantic analysis", {});
        }

        if (!package::bindImports(buildOptions.context, program))
        {
            return SourceError("Failed to bind module imports", {});
        }

        if (!sema::typeCheckProgram(buildOptions.context, program))
        {
            return SourceError("Semantic analysis failed", {});
        }

        return package::writePackage(buildOptions, program);
    }

    inline std::filesystem::path buildLibraryModule(
        std::filesystem::path const& directory,
        String const& moduleName,
        std::string_view source)
    {
        std::filesystem::path const sourcePath = directory / (moduleName + String(sourceExtension));
        writeSourceFile(sourcePath, source);

        auto contextResult = CompilationSession::makeASTContextFromSources(
            {sourcePath.string()},
            moduleName,
            ModuleDeclKind::Library);
        REQUIRE(contextResult.isOk());

        ASTContext context = std::move(contextResult).unwrap();
        auto parseResult = parseSourceFiles({sourcePath.string()});
        REQUIRE(parseResult.isOk());

        std::filesystem::path const artifactPath = directory / (moduleName + String(moduleExtension));
        package::PackageBuildOptions buildOptions;
        buildOptions.context = std::move(context);
        buildOptions.outputPath = artifactPath.string();

        auto buildResult = analyzeAndWritePackage(buildOptions, *std::move(parseResult).unwrap());
        if (!buildResult.isOk())
        {
            FAIL("Package build failed: " << buildResult.error().message);
        }
        REQUIRE(std::filesystem::is_regular_file(artifactPath));
        return artifactPath;
    }

    inline Module loadBuiltModule(std::filesystem::path const& artifactPath)
    {
        ASTContext loadContext(ModuleDecl{.name = "Client"});
        auto readResult = package::readPackageIntoContextFromPath(loadContext, artifactPath.string());
        if (readResult.isError())
        {
            FAIL(readResult.error().message);
        }

        return std::move(readResult).unwrap();
    }

    inline InterfaceDeclRecord const* findInterfaceDecl(
        Module const& module,
        StringView name,
        StringView scopeName = {})
    {
        for (InterfaceDeclRecord const& record : module.interfaceDecls)
        {
            if (module.lookupIdentifier(record.nameId) != name)
            {
                continue;
            }

            if (scopeName.empty())
            {
                if (record.parentScopeId != 0)
                {
                    continue;
                }
            }
            else if (module.lookupParentScopeName(record.parentScopeId) != scopeName)
            {
                continue;
            }

            return &record;
        }

        return nullptr;
    }

    inline bool hasInterfaceTopLevel(Module const& module, StringView name)
    {
        return findInterfaceDecl(module, name) != nullptr;
    }

    inline bool analyzeLibraryModule(
        Array<String> const& sourcePaths,
        String const& moduleName,
        String* errorMessage = nullptr,
        Array<String> const& searchPaths = {},
        Array<Diagnostic>* outDiagnostics = nullptr)
    {
        rune::testing::configureModulePathsForTests();

        auto contextResult = CompilationSession::makeASTContextFromSources(
            sourcePaths,
            moduleName,
            ModuleDeclKind::Library);
        if (contextResult.isError())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = contextResult.error().message;
            }

            return false;
        }

        ASTContext context = std::move(contextResult).unwrap();
        Array<String> resolvedSearchPaths = searchPaths;
        String const artifactPath = PackagePaths::artifactDirectory().string();
        bool hasArtifactPath = false;
        for (String const& path : resolvedSearchPaths)
        {
            if (path == artifactPath)
            {
                hasArtifactPath = true;
                break;
            }
        }

        if (!hasArtifactPath)
        {
            resolvedSearchPaths.push_back(artifactPath);
        }

        context.getModuleDecl().searchPaths = std::move(resolvedSearchPaths);

        auto parseResult = parseSourceFiles(sourcePaths);
        if (parseResult.isError())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = parseResult.error().message;
            }

            return false;
        }

        auto program = std::move(parseResult).unwrap();

        if (!sema::prepareModule(context, *program))
        {
            if (outDiagnostics != nullptr)
            {
                *outDiagnostics = context.getDiagnostics().all();
            }

            if (errorMessage != nullptr && context.getDiagnostics().hasErrors())
            {
                *errorMessage = context.getDiagnostics().all().front().message;
            }

            return false;
        }

        if (!package::bindImports(context, *program))
        {
            if (outDiagnostics != nullptr)
            {
                *outDiagnostics = context.getDiagnostics().all();
            }

            if (errorMessage != nullptr && context.getDiagnostics().hasErrors())
            {
                *errorMessage = context.getDiagnostics().all().front().message;
            }

            return false;
        }

        if (!sema::typeCheckProgram(context, *program))
        {
            if (outDiagnostics != nullptr)
            {
                *outDiagnostics = context.getDiagnostics().all();
            }

            if (errorMessage != nullptr && context.getDiagnostics().hasErrors())
            {
                *errorMessage = context.getDiagnostics().all().front().message;
            }

            return false;
        }

        if (outDiagnostics != nullptr)
        {
            *outDiagnostics = context.getDiagnostics().all();
        }

        return true;
    }

    inline void requireLibraryAnalysisOk(
        Array<String> const& sourcePaths,
        String const& moduleName,
        Array<String> const& searchPaths = {})
    {
        String errorMessage;
        if (!analyzeLibraryModule(sourcePaths, moduleName, &errorMessage, searchPaths))
        {
            FAIL("Semantic analysis failed: " << errorMessage);
        }
    }

    inline void requireLibraryAnalysisFails(
        Array<String> const& sourcePaths,
        String const& moduleName,
        std::string_view expectedSubstring = {},
        Array<String> const& searchPaths = {})
    {
        String errorMessage;
        REQUIRE_FALSE(analyzeLibraryModule(sourcePaths, moduleName, &errorMessage, searchPaths));
        if (!expectedSubstring.empty())
        {
            INFO("Error: " << errorMessage);
            REQUIRE(errorMessage.find(expectedSubstring) != String::npos);
        }
    }

    inline constexpr std::string_view accessLibSource = R"(
public static Constants {
    public const answer: Int = 42

    public func twice(value: Int) -> Int {
        return value + value
    }
}

public struct PublicBox {
    public let value: Int
    private let secret: Int
}

private struct PrivateBox {
    let value: Int
}

internal struct InternalBox {
    let value: Int
}

public func publicApi() -> Int {
    return 0
}

private func privateApi() -> Int {
    return 0
}

internal func internalApi() -> Int {
    return 0
}

func main() -> Int {
    return 0
}
)";
}
