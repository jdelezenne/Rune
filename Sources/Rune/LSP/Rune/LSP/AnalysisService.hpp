#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Macros.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/SymbolTable.hpp"

namespace rune::lsp
{
    struct ParseDiagnostic final
    {
        String message;
        SourceLocation location;
        String id;
    };

    struct AnalysisResult final
    {
        ASTContext context;
        sema::SymbolTable symbols;
        UniquePtr<parser::Program> program;
        Array<ParseDiagnostic> parseErrors;
        Optional<String> fatalError;
        bool readyForQuery{false};

        AnalysisResult() = default;

        AnalysisResult(AnalysisResult&& other) noexcept
            : context(std::move(other.context))
            , symbols(std::move(other.symbols))
            , program(std::move(other.program))
            , parseErrors(std::move(other.parseErrors))
            , fatalError(std::move(other.fatalError))
            , readyForQuery(other.readyForQuery)
        {
            other.readyForQuery = false;
            symbols.rebindContext(context);
        }

        AnalysisResult& operator=(AnalysisResult&& other) noexcept
        {
            if (this != &other)
            {
                context = std::move(other.context);
                symbols = std::move(other.symbols);
                program = std::move(other.program);
                parseErrors = std::move(other.parseErrors);
                fatalError = std::move(other.fatalError);
                readyForQuery = other.readyForQuery;
                other.readyForQuery = false;
                symbols.rebindContext(context);
            }

            return *this;
        }

        RUNE_NOT_COPYABLE(AnalysisResult)
    };

    class AnalysisService final
    {
    public:
        explicit AnalysisService(Array<String> moduleSearchPaths = {})
            : moduleSearchPaths(std::move(moduleSearchPaths))
        {
        }

        AnalysisResult analyze(
            String const& filePath,
            String const& text,
            Array<String> const& workspaceRoots) const;

    private:
        Array<String> moduleSearchPaths;
    };
}
