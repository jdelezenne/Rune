#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/DiagnosticEngine.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace ErrorHandler
    {
        /// Formatting options for diagnostic output.
        struct FormatOptions final
        {
            bool useColors = true;      ///< Use ANSI colors when supported.
            uint32 contextLines = 2;    ///< Number of source context lines to show.
            bool showSourceCode = true; ///< Include source snippets in diagnostics.
        };

        /// Prints a source-aware error with optional source context.
        /// @param error Error value to display.
        /// @param options Formatting options.
        void printError(
            SourceError const& error,
            FormatOptions const& options = {});

        /// Prints each diagnostic with source context (one block per error).
        void printDiagnostics(
            Array<Diagnostic> const& diagnostics,
            FormatOptions const& options = {});

        /// Prints an error at a source location with optional source context.
        /// @param location Source location of the error.
        /// @param message Error message.
        /// @param options Formatting options.
        void printError(
            SourceLocation const& location,
            String const& message,
            FormatOptions const& options = {});

        /// Prints a warning at a source location with optional source context.
        /// @param location Source location of the warning.
        /// @param message Warning message.
        /// @param options Formatting options.
        void printWarning(
            SourceLocation const& location,
            String const& message,
            FormatOptions const& options = {});
    }
}
