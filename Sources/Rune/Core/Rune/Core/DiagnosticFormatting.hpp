#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/DiagnosticEngine.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    /// Joins error diagnostic messages (warnings omitted) with newlines.
    String joinErrorDiagnosticMessages(Array<Diagnostic> const& diagnostics);

    /// Builds a SourceError from collected diagnostics (all error messages; primary at first error location).
    SourceError sourceErrorFromDiagnostics(Array<Diagnostic> const& diagnostics, StringView fallbackMessage = {});

    /// Converts a Result/SourceError value into a structured diagnostic.
    Diagnostic diagnosticFromSourceError(SourceError const& error);

    /// Appends a SourceError as a diagnostic entry.
    void appendSourceError(Array<Diagnostic>& diagnostics, SourceError const& error);
}
