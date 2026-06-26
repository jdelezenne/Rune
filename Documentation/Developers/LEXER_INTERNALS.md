# Lexer Internals

> **Scope:** `rune::lexer` compiler library  
> **Location:** `Code/Rune/Lexer/`

This document describes the internals of the Rune lexer, which is responsible for tokenizing Rune source code into a stream of tokens. It is the first stage of the compiler front-end.

## Overview

The `rune::lexer` namespace provides the `Lexer` class, which performs lexical analysis. The lexer depends only on the `rune` core library (for basic types like `String`, `Result`, and `UTF8` handling).

The lexer is used in two primary ways:
1. **Tokenization**: Using `tokenize()` or `tokenizeFromBuffer()` to process an entire file into an `Array<Token>` in one go.
2. **Streaming**: Using `nextToken()` to pull tokens on demand.

## State and Cursor Management

The lexer holds the source code as a `String` (UTF-8 encoded) and maintains a cursor to track the current position.

```cpp
uint32 current = 0;       // Current byte position in source.
uint32 start = 0;         // Start byte position of the current token.
uint32 currentLine = 1;   // Current line number (1-based).
uint32 currentColumn = 1; // Current column in codepoints (1-based).
uint32 startLine = 1;     // Line at which the current token started.
uint32 startColumn = 1;   // Column at which the current token started.
```

When scanning a token, `start` is synchronized with `current`. As characters are consumed, `current`, `currentLine`, and `currentColumn` are updated. 

## UTF-8 and Codepoint Scanning

Rune source files are treated as UTF-8 text. The lexer decodes and consumes UTF-8 codepoints using `rune::UTF8`.

The core cursor operations are:
- `advance()`: Decodes the codepoint at `current`, advances `current` by the byte length of the codepoint, updates line and column, and returns the codepoint.
- `peek()`: Decodes and returns the codepoint at `current` without advancing.
- `peekNext()`: Looks one codepoint ahead without advancing.
- `match(expected)`: Advances only if the next codepoint matches the `expected` parameter.

## Token Creation

Tokens are represented by the `Token` struct. A token contains:
- `TokenType` (e.g., `Identifier`, `KeywordFunc`, `Integer`, `Plus`).
- `SourceLocation` (file name, line, column, byte span).
- `String` lexeme (the exact source text slice).
- `TokenValue` (the parsed literal value for strings, ints, floats, etc.).

The lexer provides several overloaded `makeToken` methods. For instance, when an integer is scanned, `makeToken(TokenType, uint64)` is used to attach the actual parsed numeric value to the token. This prevents the parser from needing to re-parse the string later in the pipeline.

## Scanning Procedures

The `scanToken()` method is the main dispatcher. It skips whitespace (using `skipWhitespace()`) and then switches on the next codepoint to determine the token kind.

### Identifiers and Keywords
`scanIdentifier()` reads a sequence of valid identifier characters (alphanumeric and underscores). Once the lexeme is collected, `getIdentifierType()` checks if the string matches a known keyword (e.g., `func`, `struct`, `if`). If it does, a keyword token is emitted; otherwise, an `Identifier` token is emitted.

### Numbers
Numeric scanning is dispatched through `scanNumber(firstDigit)` because Rune supports multiple formats:
- **Hexadecimal** (`0x...`): Scanned by `scanHexNumber()` and `scanHexFloatNumber()`.
- **Binary** (`0b...`): Scanned by `scanBinaryNumber()`.
- **Octal** (`0o...`): Scanned by `scanOctalNumber()`.
- **Decimal/Float**: `scanDecimalNumber()` and `scanDecimalNumberWithLeadingDot()`. It handles digit separators (`_`), decimal points, and scientific exponent notation (`e` or `E`).

### Strings
String literals have several forms:
- **Standard Strings** (`"..."`): `scanString()` handles normal string literals and processes escape sequences via `parseUnicodeEscape()` (like `\n`, `\t`, `\u{XXXX}`).
- **Multiline Strings** (`"""..."""`): `scanMultilineString()` handles block strings. The `stripIndentation` helper is used to strip the common leading indentation from all lines based on the closing delimiter's position.
- **Raw Strings** (`#"..."#`): `scanRawString(delimiterCount)` matches strings where escape sequences are ignored unless accompanied by the matching number of `#` characters.
- **Characters** (`'x'`): `scanCharacter()` parses a single Unicode scalar value.

### Comments
When `LexerConfiguration::includeComments` is `false` (the default), the lexer consumes comments without emitting tokens. Set `includeComments` to `true` for LSP and other tooling that needs comment tokens in the stream.
- `scanLineComment()`: Consumes until `\n`.
- `scanBlockComment()`: Consumes until `*/`, supporting nested block comments.

## Error Handling

When the lexer encounters invalid syntax (e.g., an unterminated string, invalid digit, or unknown codepoint), it does not throw C++ exceptions. Instead, it returns a `Result<Token>` representing failure.

```cpp
Result<Token> makeErrorResult(String const& message, StringView diagnosticId) const;
```

This ensures the compiler can capture the exact `SourceLocation` and provide a high-quality diagnostic message before parsing even begins. The lexer can also provide context for an error message using `getErrorContext()`, which extracts the relevant surrounding source lines.

## Source Locations

Rune can lex from physical files or in-memory strings (e.g., during tests or LSP interactions). The `SourceLocationType` enum dictates how locations are recorded:
- `File`: The location references a `sourceFileName`.
- `String`: No file name is attached.

`SourceLocation` objects are used extensively by `rune::DiagnosticEngine` and the LSP to pinpoint errors or definitions accurately.
