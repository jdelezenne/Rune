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

        /// RAII scope that suppresses all diagnostic emission while alive.
        ///
        /// Overload resolution is speculative: candidates are evaluated to see
        /// which one matches, and a failing candidate must not leak diagnostics
        /// (e.g. materializing a literal argument against a non-matching
        /// candidate's parameter type). The resolver evaluates candidates inside
        /// a SuppressionScope, then re-checks the committed overload outside any
        /// scope so its real diagnostics surface. This is the standard
        /// speculative-then-commit model used by production type checkers.
        class [[nodiscard]] SuppressionScope final
        {
        public:
            explicit SuppressionScope(DiagnosticEngine& owner, bool active = true)
                : engine(active ? &owner : nullptr)
            {
                if (engine != nullptr)
                {
                    ++engine->suppressionDepth;
                }
            }

            ~SuppressionScope()
            {
                if (engine != nullptr)
                {
                    --engine->suppressionDepth;
                }
            }

            SuppressionScope(SuppressionScope&& other) noexcept : engine(other.engine)
            {
                other.engine = nullptr;
            }

            SuppressionScope(SuppressionScope const&) = delete;
            SuppressionScope& operator=(SuppressionScope const&) = delete;
            SuppressionScope& operator=(SuppressionScope&&) = delete;

        private:
            DiagnosticEngine* engine;
        };

        SuppressionScope suppress(bool active = true)
        {
            return SuppressionScope(*this, active);
        }

        bool isSuppressed() const
        {
            return suppressionDepth > 0;
        }

    private:
        void emplaceDiagnostic(Diagnostic diagnostic, bool isError);

        Array<Diagnostic> diagnostics;
        usize numErrors{0};
        int suppressionDepth{0};
    };
}
