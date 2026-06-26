#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    /// Related location attached to a primary diagnostic (related-note style).
    struct DiagnosticNote final
    {
        SourceLocation location;
        String message;
    };

    struct Diagnostic final
    {
        SourceLocation location;
        String message;
        String id;
        Array<DiagnosticNote> notes;
        String fixIt;
        bool isError{true};
    };

    /// Collects diagnostics for one compilation.
    class DiagnosticEngine final
    {
    public:
        void reportError(SourceLocation location, String message);
        void reportError(SourceLocation location, String message, String id);
        void reportError(SourceLocation location, String message, StringView id);
        void reportError(
            SourceLocation location,
            String message,
            String id,
            Array<DiagnosticNote> notes,
            String fixIt = {});

        void reportWarning(SourceLocation location, String message);
        void reportWarning(SourceLocation location, String message, String id);
        void reportWarning(
            SourceLocation location,
            String message,
            String id,
            Array<DiagnosticNote> notes,
            String fixIt = {});

        Array<Diagnostic> const& all() const
        {
            return diagnostics;
        }

        bool hasErrors() const
        {
            return numErrors > 0;
        }

        usize errorCount() const
        {
            return numErrors;
        }

        void clear();

    private:
        void emplaceDiagnostic(Diagnostic diagnostic, bool isError);

        Array<Diagnostic> diagnostics;
        usize numErrors{0};
    };
}
