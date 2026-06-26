#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Lexer/Token.hpp"

namespace rune::lsp
{
    struct DocumentPosition final
    {
        uint32 line = 0;
        uint32 character = 0;
    };

    /// Converts an LSP position (0-based) to a Rune position (1-based line/column).
    DocumentPosition toRunePosition(uint32 lspLine, uint32 lspCharacter);

    /// Converts an LSP position using document text for UTF-16 column mapping.
    DocumentPosition toRunePosition(String const& documentText, uint32 lspLine, uint32 lspCharacter);

    /// Returns true when @p runeLine runeColumn fall within @p location.
    bool containsPosition(SourceLocation const& location, uint32 runeLine, uint32 runeColumn);

    /// Returns true when @p runeLine runeColumn fall within @p token location.
    bool containsTokenLocation(lexer::Token const& token, uint32 runeLine, uint32 runeColumn);

    /// Returns a span size used to prefer the smallest matching node at a position.
    uint32 positionSpanSize(SourceLocation const& location);

    /// Converts a filesystem path to a file URI.
    String pathToUri(String const& path);

    /// Converts a file URI to a normalized filesystem path.
    Optional<String> uriToPath(String const& uri);

    /// Builds a file URI from a source location.
    String locationToUri(SourceLocation const& location);
}
