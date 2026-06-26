#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Lexer/Token.hpp"

namespace rune::parser
{
    class Expression;
    class BinaryExpression;
    class UnaryExpression;

    /// Binary operators supported in expressions.
    enum class BinaryOperator : uint8
    {
        Add,              ///< Addition operator (`+`).
        Subtract,         ///< Subtraction operator (`-`).
        Multiply,         ///< Multiplication operator (`*`).
        Divide,           ///< Division operator (`/`).
        Modulo,           ///< Modulo operator (`%`).
        Equal,            ///< Equality comparison operator (`==`).
        NotEqual,         ///< Inequality comparison operator (`!=`).
        LessThan,         ///< Less than operator (`<`).
        LessThanEqual,    ///< Less than or equal operator (`<=`).
        GreaterThan,      ///< Greater than operator (`>`).
        GreaterThanEqual, ///< Greater than or equal operator (`>=`).
        Spaceship,        ///< Three-way comparison/spaceship operator (`<=>`).
        LogicalAnd,       ///< Logical AND operator (`&&` or `and`).
        LogicalOr,        ///< Logical OR operator (`||` or `or`).
        BitwiseAnd,       ///< Bitwise AND operator (`&`).
        BitwiseOr,        ///< Bitwise OR operator (`|`).
        BitwiseXor,       ///< Bitwise XOR operator (`^`).
        LeftShift,        ///< Bitwise left shift operator (`<<`).
        RightShift,       ///< Bitwise right shift operator (`>>`).
        AddAssign,        ///< Compound addition assignment operator (`+=`).
        SubtractAssign,   ///< Compound subtraction assignment operator (`-=`).
        MultiplyAssign,   ///< Compound multiplication assignment operator (`*=`).
        DivideAssign,     ///< Compound division assignment operator (`/=`).
        ModuloAssign,     ///< Compound modulo assignment operator (`%=`).
        BitwiseAndAssign, ///< Compound bitwise AND assignment operator (`&=`).
        BitwiseOrAssign,  ///< Compound bitwise OR assignment operator (`|=`).
        BitwiseXorAssign, ///< Compound bitwise XOR assignment operator (`^=`).
        LeftShiftAssign,  ///< Compound bitwise left shift assignment operator (`<<=`).
        RightShiftAssign, ///< Compound bitwise right shift assignment operator (`>>=`).
        Assign,           ///< Assignment operator (`=`).
    };

    /// Unary operators supported in expressions.
    enum class UnaryOperator : uint8
    {
        Plus,               ///< Unary plus operator (`+`).
        Negate,             ///< Arithmetic negation operator (`-`).
        LogicalNot,         ///< Logical negation operator (`!` or `not`).
        BitwiseNot,         ///< Bitwise negation operator (`~`).
        ImmutableReference, ///< Immutable reference operator (`&`).
        MutableReference,   ///< Mutable reference operator (`&mut`).
        Dereference,        ///< Dereference operator (`*`).
    };

    /// Indicates what kind of variable name is being referenced.
    enum class VariableNameKind : uint8
    {
        Identifier, ///< A standard identifier.
        Self,       ///< The `self` keyword reference.
        Super,      ///< The `super` keyword reference.
        Macro,      ///< A built-in macro name (#name).
    };

    /// Maps a lexer token type to its corresponding binary operator.
    /// @param type The token type.
    /// @return The binary operator if valid, std::nullopt otherwise.
    Optional<BinaryOperator> binaryOperatorFromToken(lexer::TokenType type);

    /// Maps a lexer token type to its corresponding unary operator.
    /// @param type The token type.
    /// @return The unary operator if valid, std::nullopt otherwise.
    Optional<UnaryOperator> unaryOperatorFromToken(lexer::TokenType type);

    /// Gets a string representation of the binary operator.
    /// @param op The binary operator.
    /// @return The string representation.
    StringView describeBinaryOperator(BinaryOperator op);

    /// Gets a string representation of the unary operator.
    /// @param op The unary operator.
    /// @return The string representation.
    StringView describeUnaryOperator(UnaryOperator op);

    /// Creates a binary expression AST node.
    /// @param left The left operand.
    /// @param operatorToken The operator token.
    /// @param right The right operand.
    /// @return The constructed BinaryExpression node.
    UniquePtr<BinaryExpression> makeBinaryExpression(
        UniquePtr<Expression> left,
        lexer::Token const& operatorToken,
        UniquePtr<Expression> right);

    /// Creates a prefix unary expression AST node.
    /// @param operatorToken The operator token.
    /// @param operand The operand expression.
    /// @return The constructed prefix UnaryExpression node.
    UniquePtr<UnaryExpression> makePrefixUnaryExpression(
        lexer::Token const& operatorToken,
        UniquePtr<Expression> operand);

    /// Creates a postfix unary expression AST node.
    /// @param operatorToken The operator token.
    /// @param operand The operand expression.
    /// @return The constructed postfix UnaryExpression node.
    UniquePtr<UnaryExpression> makePostfixUnaryExpression(
        lexer::Token const& operatorToken,
        UniquePtr<Expression> operand);
}
