#pragma once

#include "Testing/ModulePathsTestSetup.hpp"
#include <Rune/Core/Result.hpp>
#include <Rune/Lexer/Lexer.hpp>
#include <Rune/Parser/AST.hpp>
#include <Rune/Parser/Parser.hpp>
#include <Rune/SemanticAnalyzer/SemanticAnalyzer.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

namespace rune::testing
{
    // Global flag set by custom test main when --verbose is passed
    extern bool g_verboseMode;

    /**
     * @brief Check if verbose mode is enabled via command line flag or environment variable
     * @return True if --verbose flag was passed or RUNE_TEST_VERBOSE environment variable is set
     */
    inline bool IsVerboseModeEnabled()
    {
        // Check command line flag first
        if (g_verboseMode)
        {
            return true;
        }

        // Fall back to environment variable
        char const* envVar = std::getenv("RUNE_TEST_VERBOSE");
        return envVar != nullptr && envVar[0] != '\0';
    }

    /**
     * @brief Create a SemanticAnalyzer instance with verbose mode enabled if requested
     * @return SemanticAnalyzer instance configured with verbose mode based on --verbose flag or environment variable
     */
    inline SemanticAnalyzer::SemanticAnalyzer CreateSemanticAnalyzer()
    {
        return SemanticAnalyzer::SemanticAnalyzer(IsVerboseModeEnabled());
    }

    /**
     * @brief Helper class for creating and running Rune tests
     */
    class TestHelper final
    {
    public:
        /**
         * @brief Tokenize and parse source code
         * @param code The source code to parse
         * @return Result containing the parsed Program or error
         */
        static Result<UniquePtr<parser::Program>> ParseCode(String const& code)
        {
            lexer::Lexer lexer;
            auto tokenResult = lexer.tokenize(code, lexer::SourceLocationType::String);

            if (tokenResult.isError())
            {
                return tokenResult.template asError<UniquePtr<parser::Program>>();
            }

            parser::Parser parser(tokenResult.value());
            return parser.parse();
        }

        /**
         * @brief Tokenize source code
         * @param code The source code
         * @return Result containing array of tokens or error
         */
        static Result<Array<lexer::Token>> TokenizeCode(String const& code)
        {
            lexer::Lexer lexer;
            return lexer.tokenize(code, lexer::SourceLocationType::String);
        }

        /**
         * @brief Check if code contains a specific token type
         * @param code The source code
         * @param tokenType The token type to search for
         * @return True if the token type is found
         */
        static bool ContainsTokenType(String const& code, lexer::TokenType tokenType)
        {
            auto result = TokenizeCode(code);
            if (result.isError())
            {
                return false;
            }

            auto tokens = result.value();
            for (const auto& token : tokens)
            {
                if (token.type == tokenType)
                {
                    return true;
                }
            }

            return false;
        }

        /**
         * @brief Count occurrences of a specific token type
         * @param code The source code
         * @param tokenType The token type to count
         * @return Number of occurrences
         */
        static usize CountTokenType(String const& code, lexer::TokenType tokenType)
        {
            auto result = TokenizeCode(code);
            if (result.isError())
            {
                return 0;
            }

            auto tokens = result.value();
            usize count = 0;
            for (const auto& token : tokens)
            {
                if (token.type == tokenType)
                {
                    count++;
                }
            }

            return count;
        }

        /**
         * @brief Perform semantic analysis on source code with automatic error reporting
         * @param code The source code to analyze
         * @return Result containing AnnotatedAST or error (errors are automatically reported via FAIL)
         */
        static Result<SemanticAnalyzer::AnnotatedAST> AnalyzeCode(String const& code)
        {
            lexer::Lexer lexer;
            auto tokenResult = lexer.tokenize(code, lexer::SourceLocationType::String);

            if (tokenResult.isError())
            {
                FAIL("Lexer error: " << tokenResult.error().message);
            }

            parser::Parser parser(tokenResult.value());
            auto parseResult = parser.parse();

            if (parseResult.isError())
            {
                FAIL("Parser error: " << parseResult.error().message);
            }

            bool verbose = IsVerboseModeEnabled();
            SemanticAnalyzer::SemanticAnalyzer analyzer(verbose);
            auto analysisResult = analyzer.analyze(std::move(parseResult.value()));

            if (analysisResult.isError())
            {
                String errorMsg = analysisResult.error().message;
                // Also get individual errors from the analyzer for more details
                auto& errors = analyzer.getErrors();

                if (!errors.empty())
                {
                    errorMsg += "\nDetailed errors:";
                    for (auto const& error : errors)
                    {
                        errorMsg += "\n  - " + error.message +
                                    " at line " + std::to_string(error.location.line) +
                                    ", column " + std::to_string(error.location.column);
                    }
                }
                FAIL("Semantic analysis failed: " << errorMsg);
            }

            return analysisResult;
        }

        /**
         * @brief Check if semantic analysis succeeds (returns true/false without reporting errors)
         * @param code The source code to analyze
         * @return True if analysis succeeds, false otherwise
         */
        static bool AnalyzeSucceeds(String const& code)
        {
            lexer::Lexer lexer;
            auto tokenResult = lexer.tokenize(code, lexer::SourceLocationType::String);
            if (tokenResult.isError())
            {
                return false;
            }

            parser::Parser parser(tokenResult.value());
            auto parseResult = parser.parse();
            if (parseResult.isError())
            {
                return false;
            }

            bool verbose = IsVerboseModeEnabled();
            SemanticAnalyzer::SemanticAnalyzer analyzer(verbose);
            auto analysisResult = analyzer.analyze(std::move(parseResult.value()));
            return analysisResult.isOk();
        }
    };
}
