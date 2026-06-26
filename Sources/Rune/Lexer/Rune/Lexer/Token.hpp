#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/UTF8.hpp"
#include "Rune/Core/Variant.hpp"
#include "Rune/Core/Void.hpp"

#include <format>

namespace rune::lexer
{
    /// Token kinds produced by the lexer.
    enum class TokenType
    {
        Invalid, ///< Invalid or unrecognized token.

        /// @name Literals
        /// @{
        Integer,       ///< Integer numeric literal.
        Float,         ///< Floating-point numeric literal.
        String,        ///< String literal.
        StringSegment, ///< Segment of an interpolated string.
        Character,     ///< Character literal.
        /// @}

        /// @name Identifier
        /// @{
        Identifier, ///< User-defined identifier.
        Discard,    ///< Discard wildcard `_` (not a bindable name).
        /// @}

        /// @name Keywords
        /// @{
        Struct,     ///< struct keyword.
        Func,       ///< func keyword.
        Let,        ///< let keyword.
        Var,        ///< var keyword.
        Const,      ///< const keyword.
        Static,     ///< static keyword (static member/extension modifier).
        Mutating,   ///< mutating keyword (mutating instance method modifier).
        Alias,      ///< alias keyword (type alias declaration).
        Type,       ///< type keyword (associated type member declaration).
        Distinct,   ///< distinct keyword (distinct type declaration).
        Extension,  ///< extension keyword (extension declaration).
        Interface,  ///< interface keyword.
        Extends,    ///< extends keyword (struct inheritance clause).
        Implements, ///< implements keyword (interface implementation clause).
        If,         ///< if keyword.
        Else,       ///< else keyword.
        While,      ///< while keyword.
        Loop,       ///< loop keyword.
        For,        ///< for keyword.
        In,         ///< in keyword.
        By,         ///< by keyword (for-in stride).
        Break,      ///< break keyword.
        Continue,   ///< continue keyword.
        Return,     ///< return keyword.
        True,       ///< true literal (reserved keyword).
        False,      ///< false literal (reserved keyword).
        Nil,        ///< nil literal (reserved keyword).
        And,        ///< and keyword.
        Or,         ///< or keyword.
        Not,        ///< not keyword.
        Self,       ///< self keyword.
        Super,      ///< super keyword (immediate base type in instance methods).
        SelfType,   ///< Self keyword (containing struct type).
        Import,     ///< import keyword.
        Enum,       ///< enum keyword.
        Union,      ///< union keyword.
        Match,      ///< match keyword.
        Init,       ///< init keyword.
        Final,      ///< final keyword (struct finalizer method name).
        Open,       ///< open keyword (allows struct inheritance).
        Override,   ///< override keyword (marks override of an open method).
        Public,     ///< public keyword.
        Internal,   ///< internal keyword.
        Private,    ///< private keyword.
        Operator,   ///< operator keyword.
        Literal,    ///< literal keyword (literal suffix on numeric types).
        As,         ///< as keyword.
        Is,         ///< is keyword.
        Throws,     ///< throws keyword.
        Throw,      ///< throw keyword.
        Try,        ///< try keyword.
        Catch,      ///< catch keyword.
        Make,       ///< make keyword.
        Dispose,    ///< dispose keyword.
        Defer,      ///< defer keyword.
        Async,      ///< async keyword.
        Await,      ///< await keyword.
        Eval,       ///< eval keyword.
        /// @}

        /// @name Operators
        /// @{
        Plus,             ///< + operator.
        Minus,            ///< - operator.
        Multiply,         ///< * operator.
        Divide,           ///< / operator.
        Modulo,           ///< % operator.
        Assign,           ///< = operator.
        Equal,            ///< == operator.
        NotEqual,         ///< != operator.
        LessThan,         ///< < operator.
        LessThanEqual,    ///< <= operator.
        Spaceship,        ///< <=> operator.
        GreaterThan,      ///< > operator.
        GreaterThanEqual, ///< >= operator.
        LogicalAnd,       ///< && operator (deprecated, use and keyword).
        LogicalOr,        ///< || operator (deprecated, use or keyword).
        LogicalNot,       ///< ! operator.
        NilCoalescing,    ///< ?? operator.
        /// @}

        /// @name Bitwise Operators
        /// @{
        Ampersand,  ///< & operator.
        Pipe,       ///< | operator.
        Caret,      ///< ^ operator.
        Tilde,      ///< ~ operator.
        LeftShift,  ///< << operator.
        RightShift, ///< >> operator.
        /// @}

        /// @name Compound Assignment Operators
        /// @{
        PlusAssign,       ///< += operator.
        MinusAssign,      ///< -= operator.
        MultiplyAssign,   ///< *= operator.
        DivideAssign,     ///< /= operator.
        ModuloAssign,     ///< %= operator.
        AmpersandAssign,  ///< &= operator.
        PipeAssign,       ///< |= operator.
        CaretAssign,      ///< ^= operator.
        LeftShiftAssign,  ///< <<= operator.
        RightShiftAssign, ///< >>= operator.
        /// @}

        /// @name Delimiters
        /// @{
        LeftParen,          ///< ( delimiter.
        RightParen,         ///< ) delimiter.
        LeftBrace,          ///< { delimiter.
        RightBrace,         ///< } delimiter.
        LeftBracket,        ///< [ delimiter.
        RightBracket,       ///< ] delimiter.
        Comma,              ///< , delimiter.
        Semicolon,          ///< ; delimiter.
        Colon,              ///< : delimiter.
        Arrow,              ///< -> delimiter.
        FatArrow,           ///< => delimiter.
        Dot,                ///< . delimiter.
        DotDot,             ///< .. delimiter.
        DotDotEqual,        ///< ..= delimiter.
        Ellipsis,           ///< ... delimiter (variadic / spread).
        Question,           ///< ? delimiter.
        At,                 ///< @ delimiter.
        Hash,               ///< # delimiter (built-in macro prefix).
        InterpolationStart, ///< ${ delimiter inside interpolated strings.
        InterpolationEnd,   ///< } delimiter ending an interpolation segment.
        /// @}

        /// @name Comments
        /// @{
        LineComment,  ///< // comment token.
        BlockComment, ///< /* */ comment token.
        /// @}

        /// @name Special
        /// @{
        EofToken, ///< End-of-file token.
        /// @}
    };

    /// Returns true when the token kind is a reserved keyword.
    bool isKeywordTokenType(TokenType type);

    /// Returns true when the token kind is an operator.
    bool isOperatorTokenType(TokenType type);

    /// Returns true when the token kind is a delimiter.
    bool isDelimiterTokenType(TokenType type);

    /// Returns true when the token kind is a compound-assignment operator.
    bool isCompoundAssignmentTokenType(TokenType type);

    /// Variant value carried by tokens with literal or computed values.
    using TokenValue = Variant<
        Void,
        int64,
        uint64,
        float64,
        String,
        UTF8::CodePoint,
        bool>;

    /// Lexical token with type, source lexeme, optional literal value, and source location.
    struct Token final
    {
        /// Token classification.
        TokenType type;

        /// Raw lexeme text extracted from the source.
        String lexeme;

        /// Parsed literal value when available.
        TokenValue literal;

        /// Source location of this token.
        SourceLocation location;

        /// True if this string token originated from a raw string literal ("""...""").
        bool isRawString = false;

        /// True if this numeric literal ended with a digit separator marker.
        bool hasTrailingDigitSeparator = false;

        /// Constructs a default token.
        Token() = default;

        /// Constructs a token without an explicit literal value.
        /// @param type Token classification.
        /// @param lexeme Raw lexeme text.
        /// @param location Source location of the token.
        Token(TokenType type, String lexeme, SourceLocation const& location)
            : type(type)
            , lexeme(std::move(lexeme))
            , literal(Void{})
            , location(location)
        {
        }

        /// Constructs a token with an explicit literal value.
        /// @param type Token classification.
        /// @param lexeme Raw lexeme text.
        /// @param literal Literal value.
        /// @param location Source location of the token.
        Token(TokenType type, String lexeme, TokenValue literal, SourceLocation const& location)
            : type(type)
            , lexeme(std::move(lexeme))
            , literal(std::move(literal))
            , location(location)
        {
        }

        /// Returns true when the token is a literal token kind.
        bool isLiteral() const
        {
            return type == TokenType::Integer || type == TokenType::Float ||
                   type == TokenType::String || type == TokenType::Character ||
                   type == TokenType::True || type == TokenType::False ||
                   type == TokenType::Nil;
        }

        /// Returns true when the token is a keyword token kind.
        bool isKeyword() const
        {
            return isKeywordTokenType(type);
        }

        /// Returns true when the token is an operator token kind.
        bool isOperator() const
        {
            return isOperatorTokenType(type);
        }

        /// Returns true when the token is a delimiter token kind.
        bool isDelimiter() const
        {
            return isDelimiterTokenType(type);
        }

        /// Returns true when the token is a compound-assignment operator token kind.
        bool isCompoundAssignment() const
        {
            return isCompoundAssignmentTokenType(type);
        }

        /// Returns true when the token is a comment token kind.
        bool isComment() const noexcept
        {
            return type == TokenType::LineComment || type == TokenType::BlockComment;
        }

        /// Returns true when the token is the discard wildcard `_`.
        bool isDiscard() const noexcept
        {
            return type == TokenType::Discard;
        }

        /// Returns a human-readable debug representation of the token.
        String toString() const
        {
            if (location.isSinglePosition())
            {
                return std::format(
                    "Token(type: {}, lexeme: '{}', file: {}, line: {}, column: {})",
                    static_cast<int>(type),
                    lexeme,
                    location.fileName,
                    location.line,
                    location.column);
            }
            else
            {
                return std::format(
                    "Token(type: {}, lexeme: '{}', file: {}, span: {}:{}-{}:{})",
                    static_cast<int>(type),
                    lexeme,
                    location.fileName,
                    location.line,
                    location.column,
                    location.endLine,
                    location.endColumn);
            }
        }
    };

    /// Mapping from source keyword lexemes to token kinds.
    inline HashMap<String, TokenType> const keywords = {
        {"struct", TokenType::Struct},
        {"func", TokenType::Func},
        {"let", TokenType::Let},
        {"var", TokenType::Var},
        {"const", TokenType::Const},
        {"static", TokenType::Static},
        {"mutating", TokenType::Mutating},
        {"alias", TokenType::Alias},
        {"type", TokenType::Type},
        {"distinct", TokenType::Distinct},
        {"extension", TokenType::Extension},
        {"interface", TokenType::Interface},
        {"extends", TokenType::Extends},
        {"implements", TokenType::Implements},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"loop", TokenType::Loop},
        {"for", TokenType::For},
        {"in", TokenType::In},
        {"by", TokenType::By},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue},
        {"return", TokenType::Return},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"nil", TokenType::Nil},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not},
        {"self", TokenType::Self},
        {"super", TokenType::Super},
        {"Self", TokenType::SelfType},
        {"import", TokenType::Import},
        {"enum", TokenType::Enum},
        {"union", TokenType::Union},
        {"match", TokenType::Match},
        {"init", TokenType::Init},
        {"final", TokenType::Final},
        {"open", TokenType::Open},
        {"override", TokenType::Override},
        {"public", TokenType::Public},
        {"internal", TokenType::Internal},
        {"private", TokenType::Private},
        {"operator", TokenType::Operator},
        {"literal", TokenType::Literal},
        {"as", TokenType::As},
        {"is", TokenType::Is},
        {"throws", TokenType::Throws},
        {"throw", TokenType::Throw},
        {"try", TokenType::Try},
        {"catch", TokenType::Catch},
        {"make", TokenType::Make},
        {"dispose", TokenType::Dispose},
        {"defer", TokenType::Defer},
        {"async", TokenType::Async},
        {"await", TokenType::Await},
        {"eval", TokenType::Eval},
    };

    /// Converts a token kind to a descriptive string.
    /// @param type Token kind to convert.
    /// @return Readable token kind string.
    String tokenTypeToString(TokenType type);

    /// Converts a token kind to its symbolic representation when available.
    /// @param type Token kind to convert.
    /// @return Symbol string for operator-like token kinds.
    String tokenTypeToSymbolString(TokenType type);

    /// Returns the Rune source-level type name for a literal token kind.
    /// Used in diagnostics to show the user-visible type of a literal expression.
    /// @param type Literal token kind (Integer, Float, String, Character, True, False, Nil).
    /// @return Rune type name ("Int", "Float", "String", "Char", "Bool") or empty string
    ///         when the token kind is not a literal.
    String tokenTypeLiteralTypeName(TokenType type);

    /// Returns true when the token kind is an arithmetic operator.
    /// @param type Token kind to test.
    bool isArithmeticOperator(TokenType type);

    /// Returns true when the token kind is a comparison operator.
    /// @param type Token kind to test.
    bool isComparisonOperator(TokenType type);

    /// Returns true when the token kind is a unary operator.
    /// @param type Token kind to test.
    bool isUnaryOperator(TokenType type);

    /// Returns true when the token kind is a binary operator.
    /// @param type Token kind to test.
    bool isBinaryOperator(TokenType type);

    /// Returns true when the token kind is a logical operator.
    /// @param type Token kind to test.
    bool isLogicalOperator(TokenType type);

    /// Returns true when the token kind is a bitwise operator.
    /// @param type Token kind to test.
    bool isBitwiseOperator(TokenType type);

    /// Returns true when the token kind is a compound-assignment operator.
    /// @param type Token kind to test.
    bool isCompoundAssignmentOperator(TokenType type);
}
