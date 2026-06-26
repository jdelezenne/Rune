#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/SourceManager.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/UTF8.hpp"
#include "Rune/Lexer/Token.hpp"

namespace rune::lexer
{
    /// Configuration options for the lexer.
    struct LexerConfiguration final
    {
        bool includeComments = false; ///< When true, comment tokens are included in the output (LSP/tooling).
    };

    /// Describes how the source location should be recorded for produced tokens.
    enum class SourceLocationType
    {
        File,   ///< Source originates from a file.
        String, ///< Source originates from an in-memory string.
    };

    /// Lexer that converts Rune source text into a flat sequence of tokens.
    class Lexer final
    {
    private:
        enum class LexState
        {
            Default,
            String,
            Interpolation,
        };

        struct StringState final
        {
            bool hasInterpolation = false;
        };

        struct InterpolationState final
        {
            uint32 braceDepth = 1;
        };

        SourceLocationType sourceLocationType;     ///< Whether source came from a file or a string.
        String sourceFileName;                     ///< File name recorded in token locations when sourceLocationType is File.
        String sourceCode;                         ///< Full source text being lexed.
        LexerConfiguration const& configuration;   ///< Active lexer configuration.
        SourceManager* sourceManager = nullptr;    ///< Optional external source registry (null = use global SourceContext).
        uint32 sourceId = SourceManager::noSource; ///< ID of this source in sourceManager; set during tokenization.

        uint32 current = 0;       ///< Current byte position in source.
        uint32 start = 0;         ///< Start byte position of the current token.
        uint32 currentLine = 1;   ///< Current line number (1-based).
        uint32 currentColumn = 1; ///< Current column in codepoints, not bytes (1-based).
        uint32 startLine = 1;     ///< Line at which the current token started.
        uint32 startColumn = 1;   ///< Column at which the current token started.

        Array<Token> pendingTokens;
        Array<LexState> stateStack;
        Array<StringState> stringStates;
        Array<InterpolationState> interpolationStates;

    public:
        /// Constructs a lexer with the given configuration and an optional source manager.
        ///
        /// When @p sourceManager is non-null the lexer registers each source buffer in it
        /// (and populates the global SourceContext cache via registerFile).
        /// When null the lexer falls back to populating SourceContext directly.
        ///
        /// @param configuration  Lexer configuration to use.
        /// @param sourceManager  Optional shared SourceManager for this compilation.
        explicit Lexer(LexerConfiguration const& configuration = {}, SourceManager* sourceManager = nullptr);

        /// Tokenizes the entire source text and returns all tokens.
        /// @param source Source text to tokenize.
        /// @param sourceLocationType How to record source locations in produced tokens.
        /// @return All tokens produced from the source, or an error.
        Result<Array<Token>> tokenize(String source, SourceLocationType sourceLocationType);

        /// Tokenizes in-memory source and records @p fileName in produced token locations.
        /// @param sourceCode Source text to tokenize.
        /// @param fileName File path recorded in token locations.
        /// @return All tokens produced from the source, or an error.
        Result<Array<Token>> tokenizeFromBuffer(String sourceCode, String fileName);

        /// Scans and returns the next token from the current position.
        /// @return The next token, or an error if the source is malformed.
        Result<Token> nextToken();

        /// Returns true when the lexer has consumed all source input.
        bool isAtEnd() const;

        /// Returns the current line number (1-based).
        uint32 getCurrentLine() const
        {
            return currentLine;
        }

        /// Returns the current column number in codepoints (1-based).
        uint32 getCurrentColumn() const
        {
            return currentColumn;
        }

        /// Returns the source location at the current cursor position.
        SourceLocation getCurrentLocation() const;

        /// Returns the source location spanning the current token being scanned.
        SourceLocation getTokenLocation() const;

        /// Returns a snippet of source context around the given line and column, suitable for error messages.
        /// @param line Line number (1-based).
        /// @param column Column number in codepoints (1-based).
        /// @return Source context string.
        String getErrorContext(uint32 line, uint32 column) const;

    private:
        /// @name Codepoint Scanning
        /// @{

        /// Consumes the current codepoint and advances the cursor.
        /// @return The consumed codepoint.
        UTF8::CodePoint advance();

        /// Returns the codepoint at the current cursor without consuming it.
        /// @return Current codepoint, or zero at end of input.
        UTF8::CodePoint peek() const;

        /// Returns the codepoint one position ahead of the cursor without consuming it.
        /// @return Next codepoint, or zero when unavailable.
        UTF8::CodePoint peekNext() const;

        /// Consumes the current codepoint if it matches expected.
        /// @param expected Codepoint to match.
        /// @return True when the codepoint matched and was consumed.
        bool match(UTF8::CodePoint expected);

        /// @}

        /// @name Token Creation
        /// @{

        /// Creates a token of the given type spanning the current lexeme.
        /// @param type Token kind.
        /// @return Constructed token.
        Token makeToken(TokenType type) const;

        /// Creates a token with a generic literal value.
        /// @param type Token kind.
        /// @param literal Literal value.
        /// @return Constructed token.
        Token makeToken(TokenType type, TokenValue literal) const;

        /// Creates a token with a signed integer literal value.
        /// @param type Token kind.
        /// @param literal Signed integer literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, int64 literal) const;

        /// Creates a token with an unsigned integer literal value.
        /// @param type Token kind.
        /// @param literal Unsigned integer literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, uint64 literal) const;

        /// Creates a token with a floating-point literal value.
        /// @param type Token kind.
        /// @param literal Floating-point literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, float64 literal) const;

        /// Creates a token with a string literal value.
        /// @param type Token kind.
        /// @param literal String literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, String literal) const;

        /// Creates a token with a character literal value.
        /// @param type Token kind.
        /// @param literal Unicode codepoint literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, UTF8::CodePoint literal) const;

        /// Creates a token with a boolean literal value.
        /// @param type Token kind.
        /// @param literal Boolean literal.
        /// @return Constructed token.
        Token makeToken(TokenType type, bool literal) const;

        /// Wraps an error message in a Result failure at the current location.
        /// @param message Human-readable error description.
        /// @param diagnosticId Stable diagnostic id.
        /// @return Error result.
        Result<Token> makeErrorResult(String const& message, StringView diagnosticId) const;

        /// @}

        /// @name Lexical Analysis
        /// @{

        /// Scans the next token from the current position.
        /// @return The next token, or an error if the source is malformed.
        Result<Token> scanToken();

        /// Scans a double-quoted string literal.
        /// @return String token, or an error if the literal is malformed.
        Result<Token> scanString();

        /// Scans the current string state and returns one segment token.
        /// @return String or StringSegment token, or an error if malformed.
        Result<Token> scanStringSegment();

        /// Scans one token while in interpolation state.
        /// @return Next interpolation token, or an error.
        Result<Token> scanInterpolationToken();

        /// Scans a multiline string literal.
        /// @return String token, or an error if the literal is malformed.
        Result<Token> scanMultilineString();

        /// Scans a character literal.
        /// @return Character token, or an error if the literal is malformed.
        Result<Token> scanCharacter();

        /// Dispatches to the appropriate numeric scanner based on prefix.
        /// @param firstDigit First digit codepoint already consumed.
        /// @return Numeric token, or an error if the literal is malformed.
        Result<Token> scanNumber(UTF8::CodePoint firstDigit);

        /// Scans a hexadecimal integer or float literal.
        /// @return Numeric token, or an error if the literal is malformed.
        Result<Token> scanHexNumber();

        /// Scans a binary integer literal.
        /// @return Integer token, or an error if the literal is malformed.
        Result<Token> scanBinaryNumber();

        /// Scans an octal integer literal.
        /// @return Integer token, or an error if the literal is malformed.
        Result<Token> scanOctalNumber();

        /// Scans a decimal integer or float literal.
        /// @return Numeric token, or an error if the literal is malformed.
        Result<Token> scanDecimalNumber();

        /// Scans a hexadecimal floating-point literal.
        /// @return Float token, or an error if the literal is malformed.
        Result<Token> scanHexFloatNumber();

        /// Scans a decimal float literal that begins with a dot.
        /// @return Float token, or an error if the literal is malformed.
        Result<Token> scanDecimalNumberWithLeadingDot();

        /// Scans an identifier or keyword.
        /// @return Identifier or keyword token, or an error if the source is malformed.
        Result<Token> scanIdentifier();

        /// Scans a line comment token.
        /// @return Line comment token, or an error if the comment is unterminated.
        Result<Token> scanLineComment();

        /// Scans a block comment token.
        /// @return Block comment token, or an error if the comment is unterminated.
        Result<Token> scanBlockComment();

        /// @}

        /// @name Helpers
        /// @{

        /// Advances past any whitespace at the current position.
        void skipWhitespace();

        /// Returns the source text of the token currently being scanned.
        /// @return Raw lexeme substring for the current token span.
        String getCurrentLexeme() const;

        /// Resolves an identifier string to its keyword token kind, or Identifier if not a keyword.
        /// @param identifier Identifier text.
        /// @return Matching keyword kind, or TokenType::Identifier.
        TokenType getIdentifierType(String const& identifier) const;

        /// Parses a \u{XXXX} Unicode escape sequence at the current position.
        /// @return The decoded codepoint, or an error.
        Result<UTF8::CodePoint> parseUnicodeEscape();

        /// Strips common leading indentation from a multiline string body.
        /// @param content Raw string content.
        /// @param indentLevel Number of leading whitespace codepoints to strip per line.
        /// @param openingLine Line number of the opening delimiter.
        /// @return Stripped content.
        String stripIndentation(String const& content, uint32 indentLevel, uint32 openingLine) const;

        /// Decodes an escape sequence body and appends the result to @p out.
        /// @param out Destination string under construction.
        /// @param escaped Character immediately following the backslash.
        /// @param allowDoubleQuote When true, `\"` is accepted.
        /// @param allowSingleQuote When true, `\'` is accepted.
        /// @return Success, or an error when the escape is invalid.
        Result<Void> appendDecodedEscape(String& out, UTF8::CodePoint escaped, bool allowDoubleQuote, bool allowSingleQuote);

        /// Decodes an escape sequence body to a single code point (character literals).
        /// @param escaped Character immediately following the backslash.
        /// @param allowDoubleQuote When true, `\"` is accepted.
        /// @param allowSingleQuote When true, `\'` is accepted.
        /// @return Decoded code point, or an error when the escape is invalid.
        Result<UTF8::CodePoint> decodeEscapeToCodePoint(UTF8::CodePoint escaped, bool allowDoubleQuote, bool allowSingleQuote);

        /// Tokenizes the current source buffer held by this lexer instance.
        Result<Array<Token>> tokenizeCurrentSource();

        /// @}
    };
}
