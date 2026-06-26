#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace UTF8
    {
        /// UTF-8 codepoint type (Unicode scalar value).
        using CodePoint = uint32;

        /// Invalid codepoint marker.
        constexpr CodePoint invalidCodepoint = 0xFFFFFFFD;

        /// Replacement character for invalid UTF-8 sequences.
        constexpr CodePoint replacementCharacter = 0xFFFD;

        /// Maximum valid Unicode codepoint.
        constexpr CodePoint maxCodepoint = 0x10FFFF;

        /// Result of a single UTF-8 decode operation.
        struct DecodeResult final
        {
            CodePoint codePoint; ///< Decoded Unicode scalar value.
            uint32 bytes;        ///< Number of input bytes consumed.
            bool valid;          ///< True when decoding succeeded.

            /// Constructs a decode result.
            /// @param codePoint Decoded Unicode scalar value.
            /// @param bytes Number of input bytes consumed.
            /// @param valid True when decoding succeeded.
            DecodeResult(CodePoint codePoint, uint32 bytes, bool valid = true)
                : codePoint(codePoint)
                , bytes(bytes)
                , valid(valid)
            {
            }
        };

        /// Decode a single UTF-8 codepoint from a string.
        /// @param str The string to decode from.
        /// @param offset Starting position in the string.
        /// @return DecodeResult containing the codepoint, bytes consumed, and validity.
        DecodeResult decode(String const& str, usize offset);
        DecodeResult decode(StringView str, usize offset);

        /// Encode a Unicode codepoint to UTF-8.
        /// @param codepoint The codepoint to encode.
        /// @return UTF-8 encoded string.
        String encode(CodePoint codePoint);

        /// Check if a code point is in the ASCII range.
        constexpr bool isAscii(CodePoint codePoint)
        {
            return codePoint <= 0x7F;
        }

        /// Check if a code point is a valid Unicode identifier start character.
        /// Based on Unicode Standard Annex #31.
        bool isIdentifierStart(CodePoint codePoint);

        /// Check if a code point is a valid Unicode identifier continuation character.
        bool isIdentifierContinue(CodePoint codePoint);

        /// Check if a code point is a whitespace character.
        bool isWhitespace(CodePoint codePoint);

        /// Check if a code point is a digit.
        bool isDigit(CodePoint codePoint);

        /// Check if a code point is an ASCII hexadecimal digit.
        bool isHexDigit(CodePoint codePoint);

        /// Check if a code point is an ASCII binary digit.
        bool isBinaryDigit(CodePoint codePoint);

        /// Check if a code point is an ASCII octal digit.
        bool isOctalDigit(CodePoint codePoint);

        /// Validate that a string contains valid UTF-8.
        /// @param str The string to validate.
        /// @return true if valid UTF-8, false otherwise.
        bool validate(String const& str);

        /// Count the number of UTF-8 codepoints in a string.
        /// @param str The string to count.
        /// @return Number of codepoints (not bytes).
        usize codepointCount(String const& str);

        /// Byte offset of the codepoint at a 0-based index (clamped to line end).
        usize byteOffsetAtCodepointIndex(StringView str, uint32 codepointIndex);

        /// Byte offset for a 1-based column (lexer / SourceLocation convention).
        usize byteOffsetAtColumn(StringView str, uint32 column1Based);

        /// Terminal display width of one Unicode scalar (East Asian Width subset).
        uint32 codepointDisplayWidth(CodePoint codePoint);

        /// Display columns before a 1-based column on one source line.
        uint32 displayWidthBeforeColumn(StringView line, uint32 column1Based);

        /// Display-column span from start column through end column (both 1-based, inclusive).
        uint32 displayWidthSpan(StringView line, uint32 startColumn1Based, uint32 endColumn1Based);

        /// UTF-16 code-unit offset for LSP `character` (0-based) from a 1-based codepoint column.
        uint32 utf16OffsetAtColumn(StringView line, uint32 column1Based);
    }
}