#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/File.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace SourceContext
    {
        /// Reads a single line from a cached source file.
        /// @param fileName Name of the source file.
        /// @param lineNumber 1-based line number.
        /// @return Line content, or an empty string when the line does not exist.
        String getLine(String const& fileName, uint32 lineNumber);

        /// Reads an inclusive range of lines from a cached source file.
        /// @param fileName Name of the source file.
        /// @param startLine First 1-based line number.
        /// @param endLine Last 1-based line number.
        /// @return Lines in range, or an empty array when the file or range is unavailable.
        Array<String> getLines(String const& fileName, uint32 startLine, uint32 endLine);

        /// Reads context lines around an error location.
        /// @param fileName Name of the source file.
        /// @param errorLine 1-based line number where the error occurs.
        /// @param contextBefore Number of lines to include before errorLine.
        /// @param contextAfter Number of lines to include after errorLine.
        /// @return Context lines surrounding the error.
        Array<String> getContextLines(
            String const& fileName,
            uint32 errorLine,
            uint32 contextBefore = 2,
            uint32 contextAfter = 2);

        /// Clears the in-memory source cache.
        void clearCache();

        /// Caches source text for diagnostics (in-memory buffers, tests, LSP).
        /// @param fileName Logical file name used in SourceLocation.
        /// @param content Full source text (split into lines like File::readLines).
        void registerSource(String const& fileName, String const& content);

        /// Returns the total number of lines in a cached source file.
        /// @param fileName Name of the source file.
        /// @return Line count, or 0 when the file does not exist.
        uint32 getLineCount(String const& fileName);
    }
}
