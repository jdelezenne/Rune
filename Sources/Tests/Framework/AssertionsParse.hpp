#pragma once

#include "Framework/AssertionsSema.hpp"
#include "Framework/Pipeline.hpp"

#include "Rune/Core/DiagnosticFormatting.hpp"
#include "Rune/Core/DiagnosticIds.hpp"
#include "Rune/Parser/Parser.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

namespace rune::test
{
    inline void requireParseOk(String const& source)
    {
        auto result = parse(source);
        if (result.isError())
        {
            FAIL("Parser error: " << result.error().message);
        }

        REQUIRE(result.value()->parseDiagnostics.empty());
    }

    inline void requireParseFails(String const& source)
    {
        auto result = parse(source);
        if (result.isError())
        {
            return;
        }

        REQUIRE_FALSE(result.value()->parseDiagnostics.empty());
    }

    inline void requireParseFails(String const& source, std::string_view expectedSubstring)
    {
        auto result = parse(source);
        String message;
        if (result.isError())
        {
            message = result.error().message;
        }
        else
        {
            REQUIRE_FALSE(result.value()->parseDiagnostics.empty());
            message = result.value()->parseDiagnostics.front().message;
        }

        if (!expectedSubstring.empty())
        {
            INFO("Error: " << message);
            REQUIRE(message.find(expectedSubstring) != String::npos);
        }
    }

    inline void requireParseFails(String const& source, ExpectedDiagnostic expected)
    {
        auto result = parse(source);
        Array<Diagnostic> diagnostics;
        if (result.isError())
        {
            appendSourceError(diagnostics, result.error());
        }
        else
        {
            diagnostics = result.value()->parseDiagnostics;
        }

        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyErrorMatches(diagnostics, expected));
    }

    inline void requireTokenizeFails(String const& source, ExpectedDiagnostic expected)
    {
        auto tokenResult = tokenize(source);
        REQUIRE(tokenResult.isError());
        Array<Diagnostic> diagnostics;
        appendSourceError(diagnostics, tokenResult.error());
        INFO("Diagnostics:\n"
             << formatDiagnosticsForInfo(diagnostics));
        REQUIRE(anyErrorMatches(diagnostics, expected));
    }
}
