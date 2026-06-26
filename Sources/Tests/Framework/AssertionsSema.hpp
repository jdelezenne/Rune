#pragma once

#include "Framework/Pipeline.hpp"
#include "Testing/ModulePathsTestSetup.hpp"

#include "Rune/Core/DiagnosticFormatting.hpp"
#include "Rune/Core/DiagnosticIds.hpp"
#include "Rune/Core/Logging.hpp"
#include "Rune/Modules/Package/ImportBinding.hpp"
#include "Rune/Modules/Package/Paths.hpp"
#include "Rune/Modules/Package/Reader.hpp"
#include "Rune/Parser/Pipeline.hpp"
#include "Rune/Sema/Sema.hpp"

#include <catch2/catch_test_macros.hpp>

#include <initializer_list>
#include <string_view>

namespace rune::test
{
    /// Prefer `using namespace rune;` then `DiagnosticIds::sema...` (not `using rune::DiagnosticIds;`).
    namespace DiagnosticIds = ::rune::DiagnosticIds;

    /// Expected semantic error: stable id plus optional message substring.
    struct ExpectedDiagnostic final
    {
        std::string_view id;
        std::string_view messageContains;
    };

    enum class ExpectedDiagnosticSeverity
    {
        Error,
        Warning,
    };

    inline ExpectedDiagnostic expectDiagnostic(
        StringView id,
        std::string_view messageContains = {})
    {
        return ExpectedDiagnostic{.id = id, .messageContains = messageContains};
    }

    inline bool diagnosticMatches(
        Diagnostic const& diagnostic,
        ExpectedDiagnostic const& expected,
        ExpectedDiagnosticSeverity severity)
    {
        bool const isExpectedError = severity == ExpectedDiagnosticSeverity::Error;
        if (diagnostic.isError != isExpectedError)
        {
            return false;
        }

        if (!expected.id.empty() &&
            (diagnostic.id.empty() || StringView(diagnostic.id) != expected.id))
        {
            return false;
        }

        if (!expected.messageContains.empty() &&
            diagnostic.message.find(expected.messageContains) == String::npos)
        {
            return false;
        }

        return true;
    }

    inline bool anyErrorMatches(
        Array<Diagnostic> const& diagnostics,
        ExpectedDiagnostic const& expected)
    {
        for (Diagnostic const& diagnostic : diagnostics)
        {
            if (diagnosticMatches(diagnostic, expected, ExpectedDiagnosticSeverity::Error))
            {
                return true;
            }
        }

        return false;
    }

    inline bool anyWarningMatches(
        Array<Diagnostic> const& diagnostics,
        ExpectedDiagnostic const& expected)
    {
        for (Diagnostic const& diagnostic : diagnostics)
        {
            if (diagnosticMatches(diagnostic, expected, ExpectedDiagnosticSeverity::Warning))
            {
                return true;
            }
        }

        return false;
    }

    inline String formatDiagnosticsForInfo(Array<Diagnostic> const& diagnostics)
    {
        String formatted;
        for (Diagnostic const& diagnostic : diagnostics)
        {
            if (!formatted.empty())
            {
                formatted += '\n';
            }

            formatted += diagnostic.isError ? "[error] " : "[warning] ";

            if (!diagnostic.id.empty())
            {
                formatted += '[';
                formatted += diagnostic.id;
                formatted += "] ";
            }

            formatted += diagnostic.message;
        }

        return formatted;
    }

    inline Array<String> makeTestSearchPaths(Array<String> const& searchPaths)
    {
        String const artifactPath = PackagePaths::artifactDirectory().string();
        Array<String> resolvedPaths = searchPaths;

        bool hasArtifactPath = false;
        for (String const& path : resolvedPaths)
        {
            if (path == artifactPath)
            {
                hasArtifactPath = true;
                break;
            }
        }

        if (!hasArtifactPath)
        {
            resolvedPaths.push_back(artifactPath);
        }

        return resolvedPaths;
    }

    inline bool analyzeSource(
        String const& source,
        String* errorMessage = nullptr,
        Array<String> const& searchPaths = {},
        Array<Diagnostic>* outDiagnostics = nullptr,
        SemaTraceFlag traceFlags = SemaTraceFlag::None,
        Array<String> const& dependencies = {},
        Array<String> const& alwaysImported = {},
        StringView moduleName = "TestModule",
        bool requireEscape = false,
        bool allowUnsafe = false)
    {
        rune::testing::configureModulePathsForTests();

        auto parseResult = parseWithRecovery(source);
        if (parseResult.isError())
        {
            SourceError const& parseError = parseResult.error();
            if (errorMessage != nullptr)
            {
                *errorMessage = parseError.message;
            }

            if (outDiagnostics != nullptr)
            {
                outDiagnostics->clear();
                appendSourceError(*outDiagnostics, parseError);
            }

            return false;
        }

        auto program = std::move(parseResult).unwrap();
        if (!program->parseDiagnostics.empty())
        {
            bool hasParseError = false;
            for (Diagnostic const& diagnostic : program->parseDiagnostics)
            {
                if (diagnostic.isError)
                {
                    hasParseError = true;
                    break;
                }
            }

            if (hasParseError)
            {
                if (outDiagnostics != nullptr)
                {
                    *outDiagnostics = program->parseDiagnostics;
                }

                if (errorMessage != nullptr)
                {
                    *errorMessage = joinErrorDiagnosticMessages(program->parseDiagnostics);
                }

                return false;
            }
        }

        parser::findExplicitEntryPoint(*program);
        parser::synthesizeMainForTopLevelExecutableCode(*program);

        ModuleDecl moduleDecl;
        moduleDecl.name = String(moduleName);
        moduleDecl.searchPaths = makeTestSearchPaths(searchPaths);
        moduleDecl.requireEscapeChecking = requireEscape;
        moduleDecl.allowUnsafe = allowUnsafe;
        ASTContext context(std::move(moduleDecl));

        SharedPtr<StdoutLogOutput> traceOutput;
        if (traceFlags != SemaTraceFlag::None)
        {
            context.setSemaTraceFlags(traceFlags);
            traceOutput = std::make_shared<StdoutLogOutput>();
            Logger::addOutput(traceOutput);
        }

        auto const captureDiagnostics = [&]()
        {
            if (outDiagnostics != nullptr)
            {
                *outDiagnostics = context.getDiagnostics().all();
            }

            if (errorMessage != nullptr && context.getDiagnostics().hasErrors())
            {
                *errorMessage = joinErrorDiagnosticMessages(context.getDiagnostics().all());
            }

            if (traceOutput)
            {
                Logger::removeOutput(traceOutput);
            }
        };

        if (!sema::prepareModule(context, *program))
        {
            captureDiagnostics();
            return false;
        }

        if (!package::preloadImplicitDependencies(context))
        {
            captureDiagnostics();
            return false;
        }

        Array<String> effectiveDependencies;
        for (String const& depName : dependencies)
        {
            if (!depName.empty() && depName != "Rune")
            {
                effectiveDependencies.push_back(depName);
            }
        }

        for (String const& depName : effectiveDependencies)
        {
            if (depName.empty())
            {
                continue;
            }

            auto depReadResult = package::readPackageIntoContext(context, depName);
            if (depReadResult.isError())
            {
                context.getDiagnostics().reportError(
                    {},
                    depReadResult.error().message,
                    DiagnosticIds::semaModuleLoadFailed);
                captureDiagnostics();
                return false;
            }
        }

        if (!package::bindImplicitImports(context, alwaysImported))
        {
            captureDiagnostics();
            return false;
        }

        if (!package::bindImports(context, *program))
        {
            captureDiagnostics();
            return false;
        }

        if (!sema::typeCheckProgram(context, *program))
        {
            captureDiagnostics();
            return false;
        }

        if (outDiagnostics != nullptr)
        {
            *outDiagnostics = context.getDiagnostics().all();
        }

        if (traceOutput)
        {
            Logger::removeOutput(traceOutput);
        }

        return true;
    }

    inline void requireAnalysisOk(
        String const& source,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {},
        Array<String> const& alwaysImported = {},
        StringView moduleName = "TestModule")
    {
        String errorMessage;
        if (!analyzeSource(
                source,
                &errorMessage,
                searchPaths,
                nullptr,
                SemaTraceFlag::None,
                dependencies,
                alwaysImported,
                moduleName))
        {
            FAIL("Semantic analysis failed: " << errorMessage);
        }
    }

    inline void requireAnalysisOkUnsafe(
        String const& source,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {},
        Array<String> const& alwaysImported = {},
        StringView moduleName = "TestModule")
    {
        String errorMessage;
        if (!analyzeSource(
                source,
                &errorMessage,
                searchPaths,
                nullptr,
                SemaTraceFlag::None,
                dependencies,
                alwaysImported,
                moduleName,
                /* requireEscape */ false,
                /* allowUnsafe */ true))
        {
            FAIL("Semantic analysis failed: " << errorMessage);
        }
    }

    /// Like requireAnalysisOk, but with BorrowChecker escape-site enforcement turned on
    /// (equivalent to passing --require-escape to the toolchain). Use for tests that exercise
    /// the `escape` keyword; ordinary requireAnalysisOk/requireAnalysisFails leave escape-site
    /// checks off, matching the default permissive rollout state.
    inline void requireAnalysisOkWithEscape(
        String const& source,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {},
        Array<String> const& alwaysImported = {},
        StringView moduleName = "TestModule")
    {
        String errorMessage;
        if (!analyzeSource(
                source,
                &errorMessage,
                searchPaths,
                nullptr,
                SemaTraceFlag::None,
                dependencies,
                alwaysImported,
                moduleName,
                /* requireEscape */ true))
        {
            FAIL("Semantic analysis failed: " << errorMessage);
        }
    }

    /// Like requireAnalysisFails, but with BorrowChecker escape-site enforcement turned on. See
    /// requireAnalysisOkWithEscape.
    inline void requireAnalysisFailsWithEscape(
        String const& source,
        ExpectedDiagnostic expected,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(
            source,
            nullptr,
            searchPaths,
            &diagnostics,
            SemaTraceFlag::None,
            dependencies,
            {},
            "TestModule",
            /* requireEscape */ true,
            /* allowUnsafe */ false));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyErrorMatches(diagnostics, expected));
    }

    inline void requireAnalysisFails(String const& source)
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(source, nullptr, {}, &diagnostics));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(!diagnostics.empty());
        bool hasError = false;
        for (Diagnostic const& diagnostic : diagnostics)
        {
            if (diagnostic.isError)
            {
                hasError = true;
                break;
            }
        }

        REQUIRE(hasError);
    }

    inline void requireAnalysisFails(
        String const& source,
        ExpectedDiagnostic expected,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(source, nullptr, searchPaths, &diagnostics, SemaTraceFlag::None, dependencies));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyErrorMatches(diagnostics, expected));
    }

    inline void requireAnalysisFailsUnsafe(
        String const& source,
        ExpectedDiagnostic expected,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(
            source,
            nullptr,
            searchPaths,
            &diagnostics,
            SemaTraceFlag::None,
            dependencies,
            {},
            "TestModule",
            /* requireEscape */ false,
            /* allowUnsafe */ true));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyErrorMatches(diagnostics, expected));
    }

    inline void requireAnalysisFailsUnsafe(
        String const& source,
        std::string_view expectedSubstring,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        requireAnalysisFailsUnsafe(
            source,
            ExpectedDiagnostic{.messageContains = expectedSubstring},
            searchPaths,
            dependencies);
    }

    inline void requireAnalysisFails(
        String const& source,
        std::initializer_list<ExpectedDiagnostic> expectedDiagnostics,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(source, nullptr, searchPaths, &diagnostics, SemaTraceFlag::None, dependencies));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        for (ExpectedDiagnostic const& expected : expectedDiagnostics)
        {
            REQUIRE(anyErrorMatches(diagnostics, expected));
        }
    }

    inline void requireAnalysisFails(
        String const& source,
        std::string_view expectedSubstring,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        requireAnalysisFails(
            source,
            ExpectedDiagnostic{.messageContains = expectedSubstring},
            searchPaths,
            dependencies);
    }

    inline void requireAnalysisWarns(
        String const& source,
        ExpectedDiagnostic expected,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {},
        Array<String> const& alwaysImported = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE(analyzeSource(
            source,
            nullptr,
            searchPaths,
            &diagnostics,
            SemaTraceFlag::None,
            dependencies,
            alwaysImported));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyWarningMatches(diagnostics, expected));
    }

    /// Legacy: message substring only (no diagnostic id). Prefer expectDiagnostic for new tests.
    inline void requireAnalysisFails(
        String const& source,
        std::initializer_list<std::string_view> expectedSubstrings,
        Array<String> const& searchPaths = {},
        Array<String> const& dependencies = {})
    {
        Array<Diagnostic> diagnostics;
        REQUIRE_FALSE(analyzeSource(source, nullptr, searchPaths, &diagnostics, SemaTraceFlag::None, dependencies));
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        for (std::string_view expectedSubstring : expectedSubstrings)
        {
            REQUIRE(anyErrorMatches(
                diagnostics,
                ExpectedDiagnostic{.messageContains = expectedSubstring}));
        }
    }

    struct AnalysisSession final
    {
        ASTContext context{ModuleDecl{.name = "TestModule"}};
        UniquePtr<parser::Program> program;
    };

    inline bool analyzeSourceSession(
        String const& source,
        AnalysisSession& session,
        String* errorMessage = nullptr,
        Array<String> const& searchPaths = {})
    {
        rune::testing::configureModulePathsForTests();

        session.context = ASTContext(ModuleDecl{
            .name = "TestModule",
            .searchPaths = makeTestSearchPaths(searchPaths),
        });

        auto parseResult = parseWithRecovery(source);
        if (parseResult.isError())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = parseResult.error().message;
            }

            return false;
        }

        session.program = std::move(parseResult).unwrap();
        parser::findExplicitEntryPoint(*session.program);
        parser::synthesizeMainForTopLevelExecutableCode(*session.program);

        if (!sema::prepareModule(session.context, *session.program))
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = joinErrorDiagnosticMessages(session.context.getDiagnostics().all());
            }

            return false;
        }

        if (!package::preloadImplicitDependencies(session.context) ||
            !package::bindImplicitImports(session.context, {}))
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = joinErrorDiagnosticMessages(session.context.getDiagnostics().all());
            }

            return false;
        }

        if (!package::bindImports(session.context, *session.program))
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = joinErrorDiagnosticMessages(session.context.getDiagnostics().all());
            }

            return false;
        }

        if (!sema::typeCheckProgram(session.context, *session.program))
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = joinErrorDiagnosticMessages(session.context.getDiagnostics().all());
            }

            return false;
        }

        return true;
    }
}
