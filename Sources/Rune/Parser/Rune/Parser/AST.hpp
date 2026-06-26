#pragma once

#include "Rune/Parser/Operators.hpp"

#include "Rune/Core/Array.hpp"
#include "Rune/Core/DiagnosticEngine.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Lexer/Token.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

namespace rune::parser
{
    class ASTVisitor;
    class TypeExpression; ///< Forward declaration.

    /// Returns true when token is the discard wildcard `_`.
    /// @param token Token to test.
    inline bool isDiscardToken(lexer::Token const& token) noexcept
    {
        return token.isDiscard();
    }

    /// Discriminator for every AST node type.
    enum class ASTNodeKind
    {
        /// @name Program
        /// @{
        Program, ///< Root program node.
                 /// @}

        /// @name Attributes
        /// @{
        Attribute, ///< Declaration attribute such as @builtin.
                   /// @}

        /// @name Expressions
        /// @{
        LiteralExpression,
        VariableExpression,
        BinaryExpression,
        UnaryExpression,
        CallExpression,
        MemberExpression,
        OptionalMemberExpression,
        AssignExpression,
        ArrayLiteralExpression,
        IndexExpression,
        TernaryExpression,
        NilCoalescingExpression,
        IfExpression,
        MatchExpression,
        LambdaExpression,
        RangeExpression,
        TypeExpression,
        CastExpression,      ///< Type cast expression (`expr as Type`).
        TypeCheckExpression, ///< Type check expression (`expr is Type`).
        TryExpression,       ///< Try expression (`try expr`, `try? expr`, `try! expr`).
        AwaitExpression,     ///< Await expression (`await expr`).
                             /// @}

        /// @name Statements
        /// @{
        ExpressionStatement,
        BlockStatement,
        VariableDeclaration,
        IfStatement,
        GuardStatement,
        WhileStatement,
        LoopStatement,
        ReturnStatement,
        FunctionDeclaration,
        StructDeclaration,
        AliasDeclaration,
        DistinctTypeDeclaration,
        ExtensionDeclaration,
        StaticScopeDeclaration,
        AttributedDeclarationGroup,
        InterfaceDeclaration,
        TypeMemberDeclaration,
        ForStatement,
        BreakStatement,
        ContinueStatement,
        ImportStatement,
        EnumDeclaration,
        UnionDeclaration,
        MatchStatement,
        PropertyDeclaration,
        ThrowStatement,
        TryStatement,
        MakeExpression,
        DisposeStatement,
        DeferStatement,
        /// @}
    };

    /// Storage class for a local or field binding.
    enum class VariableKind
    {
        Var,   ///< Mutable variable (`var`).
        Let,   ///< Immutable variable (`let`).
        Const, ///< Compile-time constant (`const`).
    };

    /// Visibility of a declaration within and across modules.
    enum class AccessModifier
    {
        Public,   ///< Exported to other modules.
        Internal, ///< Visible within the module only.
        Private,  ///< Visible within the enclosing file only.
    };

    /// Base class for all AST nodes with source location and visitor dispatch.
    class ASTNode
    {
    protected:
        SourceLocation location;         ///< Source span for this node.
        SourceFileID sourceFileIndex{0}; ///< Index into `Program::sourceFilePaths`.

    public:
        virtual ~ASTNode() = default;

        /// Dispatches the node to a visitor implementation.
        virtual void accept(ASTVisitor& visitor) = 0;

        /// Returns the concrete node kind discriminator.
        virtual ASTNodeKind getKind() const = 0;

        /// Returns the source location of this node.
        SourceLocation const& getLocation() const
        {
            return location;
        }

        /// Replaces the source location of this node.
        /// @param location New source location.
        void setLocation(SourceLocation const& location)
        {
            this->location = location;
        }

        /// Returns the index of the source file that owns this node.
        SourceFileID getSourceFileIndex() const
        {
            return sourceFileIndex;
        }

        /// Sets the index of the source file that owns this node.
        /// @param sourceFileIndex Index into `Program::sourceFilePaths`.
        void setSourceFileIndex(SourceFileID sourceFileIndex)
        {
            this->sourceFileIndex = sourceFileIndex;
        }
    };

    /// Base class for expression AST nodes.
    class Expression : public ASTNode
    {
    };

    /// Base class for statement AST nodes.
    class Statement : public ASTNode
    {
    };

    struct AttributeArgument final
    {
        Optional<lexer::Token> label;
        lexer::Token value;
    };

    /// Source attribute such as `@builtin` or `@platform(macos)`.
    class Attribute final : public ASTNode
    {
    public:
        lexer::Token name;                  ///< Attribute name token.
        Array<AttributeArgument> arguments; ///< Optional attribute argument tokens.

        /// Constructs an attribute node.
        /// @param name Attribute name token.
        /// @param arguments Optional attribute arguments.
        explicit Attribute(lexer::Token name, Array<AttributeArgument> arguments = {})
            : name(std::move(name))
            , arguments(std::move(arguments))
        {
            setLocation(this->name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::Attribute;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Literal expression node (`42`, `"hi"`, `true`, etc.).
    class LiteralExpression final : public Expression
    {
    public:
        lexer::Token value; ///< Literal token including value.
        bool isLiteralSuffixReceiver = false;

        /// Constructs a literal expression.
        /// @param value Literal token.
        explicit LiteralExpression(lexer::Token value, bool isLiteralSuffixReceiver = false)
            : value(std::move(value))
            , isLiteralSuffixReceiver(isLiteralSuffixReceiver)
        {
            setLocation(this->value.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::LiteralExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Identifier expression node.
    class VariableExpression final : public Expression
    {
    public:
        VariableNameKind nameKind{VariableNameKind::Identifier};
        lexer::Token name; ///< Identifier or keyword token (self, super).

        explicit VariableExpression(lexer::Token value, VariableNameKind kind = VariableNameKind::Identifier)
            : nameKind(kind)
            , name(std::move(value))
        {
            setLocation(name.location);
        }

        bool isSelf() const
        {
            return nameKind == VariableNameKind::Self;
        }

        bool isSuper() const
        {
            return nameKind == VariableNameKind::Super;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::VariableExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Binary operator expression node.
    class BinaryExpression final : public Expression
    {
    public:
        UniquePtr<Expression> left; ///< Left operand.
        BinaryOperator op{BinaryOperator::Add};
        SourceLocation operatorLocation;
        UniquePtr<Expression> right; ///< Right operand.

        BinaryExpression(
            UniquePtr<Expression> leftExpression,
            BinaryOperator binaryOperator,
            SourceLocation location,
            UniquePtr<Expression> rightExpression)
            : left(std::move(leftExpression))
            , op(binaryOperator)
            , operatorLocation(location)
            , right(std::move(rightExpression))
        {
            setLocation(operatorLocation);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::BinaryExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Unary operator expression node.
    class UnaryExpression final : public Expression
    {
    public:
        UnaryOperator op{UnaryOperator::Negate};
        SourceLocation operatorLocation;
        bool isPrefix{true};
        UniquePtr<Expression> operand; ///< Operand expression.

        UnaryExpression(
            UnaryOperator unaryOperator,
            SourceLocation location,
            bool prefix,
            UniquePtr<Expression> expression)
            : op(unaryOperator)
            , operatorLocation(location)
            , isPrefix(prefix)
            , operand(std::move(expression))
        {
            setLocation(operatorLocation);
        }

        bool isPostfixForceUnwrap() const
        {
            return op == UnaryOperator::LogicalNot && !isPrefix;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::UnaryExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Type expression node, including generics, tuples, function types, and references.
    class TypeExpression final : public ASTNode
    {
    public:
        /// Reference kind for raw reference types (`*T` and `&T`).
        enum class ReferenceKind
        {
            None,      ///< Not a reference type.
            Immutable, ///< `*T` immutable reference.
            Mutable,   ///< `&T` mutable reference.
        };

        lexer::Token baseType;                            ///< Base type name token (unqualified).
        Array<lexer::Token> qualifiers;                   ///< Scope qualifier tokens, e.g. ["LibC"] for LibC.FILE.
        Array<UniquePtr<TypeExpression>> typeArguments;   ///< Generic type arguments.
        UniquePtr<TypeExpression> constraint;             ///< Generic argument constraint in extension targets.
        Array<lexer::Token> tupleElementLabels;           ///< Element labels for tuple types.
        bool isTuple;                                     ///< True for tuple types.
        bool isFunctionType;                              ///< True for function types.
        bool isAsync{false};                              ///< True for `() async -> T` function types.
        UniquePtr<TypeExpression> returnType;             ///< Return type for function types.
        ReferenceKind referenceKind{ReferenceKind::None}; ///< Reference kind when applicable.

        /// Constructs a named or generic type expression.
        TypeExpression(lexer::Token baseTypeToken, Array<UniquePtr<TypeExpression>> args = {}, bool isTupleType = false)
            : baseType(std::move(baseTypeToken))
            , typeArguments(std::move(args))
            , isTuple(isTupleType)
            , isFunctionType(false)
            , returnType(nullptr)
        {
            setLocation(baseType.location);
        }

        /// Constructs a function type expression `(T, U) -> R`.
        TypeExpression(Array<UniquePtr<TypeExpression>> paramTypes, UniquePtr<TypeExpression> returnTypeExpr)
            : baseType(lexer::Token(lexer::TokenType::Identifier, "Function", returnTypeExpr ? returnTypeExpr->location : SourceLocation{}))
            , typeArguments(std::move(paramTypes))
            , isTuple(false)
            , isFunctionType(true)
            , returnType(std::move(returnTypeExpr))
        {
            if (returnType)
            {
                setLocation(returnType->location);
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TypeExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Returns true when a type expression is the inferred inline-array placeholder `_` in type position.
    /// @param typeExpr Type expression to test.
    inline bool isInferredArraySizePlaceholder(TypeExpression const& typeExpr) noexcept
    {
        return isDiscardToken(typeExpr.baseType);
    }

    /// Returns true for fixed-size inline array syntax parsed as `InlineArray<T, N>`.
    /// @param typeExpr Type expression to test.
    inline bool isFixedInlineArrayTypeExpression(TypeExpression const& typeExpr) noexcept
    {
        return typeExpr.baseType.lexeme == "InlineArray" && typeExpr.typeArguments.size() == 2;
    }

    /// Returns true for dynamic array shorthand `[T]` parsed as `Array<T>`.
    /// @param typeExpr Type expression to test.
    inline bool isBracketDynamicArrayTypeExpression(TypeExpression const& typeExpr) noexcept
    {
        return typeExpr.baseType.lexeme == "Array" && typeExpr.typeArguments.size() == 1;
    }

    inline bool dynamicArrayTypeInfersElementType(TypeExpression const& typeExpr) noexcept
    {
        return isBracketDynamicArrayTypeExpression(typeExpr) &&
               isInferredArraySizePlaceholder(*typeExpr.typeArguments[0]);
    }

    inline bool inlineArrayTypeInfersElementType(TypeExpression const& typeExpr) noexcept
    {
        return isFixedInlineArrayTypeExpression(typeExpr) &&
               isInferredArraySizePlaceholder(*typeExpr.typeArguments[0]);
    }

    inline bool inlineArrayTypeInfersSize(TypeExpression const& typeExpr) noexcept
    {
        return isFixedInlineArrayTypeExpression(typeExpr) &&
               isInferredArraySizePlaceholder(*typeExpr.typeArguments[1]);
    }

    inline bool inlineArrayTypeRequiresLiteralInference(TypeExpression const& typeExpr) noexcept
    {
        return inlineArrayTypeInfersElementType(typeExpr) || inlineArrayTypeInfersSize(typeExpr);
    }

    /// Function call argument (can be positional or named)
    struct Argument final
    {
        std::optional<lexer::Token> name; ///< Name of the parameter (for named arguments)
        UniquePtr<Expression> value;      ///< Argument value expression
        bool isSpread;                    ///< True for ...expr spread at call site

        /// Constructor for positional argument
        explicit Argument(UniquePtr<Expression> valueExpr, bool spread = false)
            : name(std::nullopt)
            , value(std::move(valueExpr))
            , isSpread(spread)
        {
        }

        /// Constructor for named argument
        Argument(lexer::Token paramName, UniquePtr<Expression> valueExpr, bool spread = false)
            : name(std::move(paramName))
            , value(std::move(valueExpr))
            , isSpread(spread)
        {
        }

        /// Check if this is a named argument
        bool isNamed() const
        {
            return name.has_value();
        }
    };

    /// Function call expressions
    class CallExpression final : public Expression
    {
    public:
        UniquePtr<Expression> callee;
        lexer::Token parent;
        Array<Argument> arguments;
        Array<UniquePtr<TypeExpression>> typeArguments; ///< Generic type arguments (e.g., <Int> in Box<Int>())

        CallExpression(
            UniquePtr<Expression> calleeExpression,
            lexer::Token parentToken,
            Array<Argument> argumentList,
            Array<UniquePtr<TypeExpression>> typeArgs)
            : callee(std::move(calleeExpression))
            , parent(std::move(parentToken))
            , arguments(std::move(argumentList))
            , typeArguments(std::move(typeArgs))
        {
            if (!parent.location.fileName.empty() && parent.location.line != 0)
            {
                setLocation(parent.location);
            }
            else if (callee != nullptr)
            {
                setLocation(callee->getLocation());
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::CallExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Member access expressions (obj.field)
    class MemberExpression final : public Expression
    {
    public:
        UniquePtr<Expression> object; ///< nullptr for leading dot syntax (e.g., .completed)
        lexer::Token name;

        MemberExpression(UniquePtr<Expression> object, lexer::Token name)
            : object(std::move(object))
            , name(std::move(name))
        {
            setLocation(this->name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::MemberExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Optional member access expressions (obj?.field)
    class OptionalMemberExpression final : public Expression
    {
    public:
        UniquePtr<Expression> object;
        lexer::Token questionToken;
        lexer::Token name;

        OptionalMemberExpression(
            UniquePtr<Expression> object,
            lexer::Token questionToken,
            lexer::Token name)
            : object(std::move(object))
            , questionToken(std::move(questionToken))
            , name(std::move(name))
        {
            setLocation(this->questionToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::OptionalMemberExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Assignment expressions
    class AssignExpression final : public Expression
    {
    public:
        lexer::Token name;
        UniquePtr<Expression> value;

        AssignExpression(lexer::Token name, UniquePtr<Expression> valueExpression)
            : name(std::move(name))
            , value(std::move(valueExpression))
        {
            setLocation(this->name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::AssignExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Bracket literal kind: list `[a, b]` (Array or HashSet) vs map `[k: v]` / `[:]`
    enum class BracketLiteralKind
    {
        List,
        Map,
    };

    struct HashMapLiteralEntry final
    {
        UniquePtr<Expression> key;
        UniquePtr<Expression> value;

        HashMapLiteralEntry(UniquePtr<Expression> key, UniquePtr<Expression> value)
            : key(std::move(key))
            , value(std::move(value))
        {
        }
    };

    /// Bracket literal expressions: [1, 2, 3], [1, 2] as HashSet, ["a": 1], [:]
    class ArrayLiteralExpression final : public Expression
    {
    public:
        BracketLiteralKind literalKind;
        Array<UniquePtr<Expression>> elements;
        Array<lexer::Token> tupleElementLabels;
        Array<HashMapLiteralEntry> entries;
        lexer::Token leftBracket;

        ArrayLiteralExpression(
            Array<UniquePtr<Expression>> elements,
            lexer::Token leftBracket,
            BracketLiteralKind literalKind = BracketLiteralKind::List)
            : literalKind(literalKind)
            , elements(std::move(elements))
            , leftBracket(std::move(leftBracket))
        {
            setLocation(this->leftBracket.location);
        }

        ArrayLiteralExpression(
            Array<HashMapLiteralEntry> entries,
            lexer::Token leftBracket)
            : literalKind(BracketLiteralKind::Map)
            , entries(std::move(entries))
            , leftBracket(std::move(leftBracket))
        {
            setLocation(this->leftBracket.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ArrayLiteralExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Index expressions array[0]
    class IndexExpression final : public Expression
    {
    public:
        UniquePtr<Expression> object;
        UniquePtr<Expression> index;
        lexer::Token leftBracket;

        IndexExpression(
            UniquePtr<Expression> object,
            UniquePtr<Expression> index,
            lexer::Token leftBracket)
            : object(std::move(object))
            , index(std::move(index))
            , leftBracket(std::move(leftBracket))
        {
            setLocation(this->leftBracket.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::IndexExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Ternary expressions condition ? trueExpr : falseExpr
    class TernaryExpression final : public Expression
    {
    public:
        UniquePtr<Expression> condition;
        UniquePtr<Expression> trueExpression;
        UniquePtr<Expression> falseExpression;
        lexer::Token questionToken;
        lexer::Token colonToken;

        TernaryExpression(
            UniquePtr<Expression> condition,
            UniquePtr<Expression> trueExpression,
            UniquePtr<Expression> falseExpression,
            lexer::Token questionToken,
            lexer::Token colonToken)
            : condition(std::move(condition))
            , trueExpression(std::move(trueExpression))
            , falseExpression(std::move(falseExpression))
            , questionToken(std::move(questionToken))
            , colonToken(std::move(colonToken))
        {
            setLocation(this->questionToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TernaryExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Nil coalescing expressions leftExpr ?? rightExpr
    class NilCoalescingExpression final : public Expression
    {
    public:
        UniquePtr<Expression> left;
        UniquePtr<Expression> right;

        NilCoalescingExpression(
            UniquePtr<Expression> left,
            UniquePtr<Expression> right,
            SourceLocation operatorLocation)
            : left(std::move(left))
            , right(std::move(right))
        {
            setLocation(operatorLocation);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::NilCoalescingExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// If expression that reuses statement-style branches.
    class IfExpression final : public Expression
    {
    public:
        UniquePtr<Statement> statement;

        explicit IfExpression(UniquePtr<Statement> ifStatement)
            : statement(std::move(ifStatement))
        {
            if (statement != nullptr)
            {
                setLocation(statement->getLocation());
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::IfExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Match expression that reuses statement-style cases.
    class MatchExpression final : public Expression
    {
    public:
        UniquePtr<Statement> statement;

        explicit MatchExpression(UniquePtr<Statement> matchStatement)
            : statement(std::move(matchStatement))
        {
            if (statement != nullptr)
            {
                setLocation(statement->getLocation());
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::MatchExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Generic parameters (e.g., <T, U> or <T extends Hashable> or <N extends UInt>)
    struct TypeParameter final
    {
        lexer::Token name;
        UniquePtr<TypeExpression> constraint;
        UniquePtr<TypeExpression> defaultArgument;

        explicit TypeParameter(
            lexer::Token nameToken,
            UniquePtr<TypeExpression> constraintExpr = nullptr,
            UniquePtr<TypeExpression> defaultArgumentExpr = nullptr)
            : name(std::move(nameToken))
            , constraint(std::move(constraintExpr))
            , defaultArgument(std::move(defaultArgumentExpr))
        {
        }
    };

    /// Function parameters (forward declaration needed for Lambda)
    struct Parameter final
    {
        lexer::Token name;
        UniquePtr<TypeExpression> type;
        UniquePtr<Expression> defaultValue;
        bool isVariadic;

        Parameter(
            lexer::Token nameToken,
            UniquePtr<TypeExpression> typeExpr,
            UniquePtr<Expression> defaultExpr = nullptr,
            bool variadic = false)
            : name(std::move(nameToken))
            , type(std::move(typeExpr))
            , defaultValue(std::move(defaultExpr))
            , isVariadic(variadic)
        {
        }
    };

    /// Lambda expressions (x, y) -> x + y  or  (x: Int, y: Int) -> Int { return x + y }
    class LambdaExpression final : public Expression
    {
    public:
        Array<Parameter> parameters;
        UniquePtr<TypeExpression> returnType; ///< Always present (defaults to Void if not specified)
        UniquePtr<Statement> body;            ///< Can be ExpressionStatement or BlockStatement
        bool hasExplicitReturnType;

        LambdaExpression(
            Array<Parameter> parameters,
            UniquePtr<TypeExpression> returnType,
            UniquePtr<Statement> body,
            bool hasExplicitReturnType = false)
            : parameters(std::move(parameters))
            , returnType(std::move(returnType))
            , body(std::move(body))
            , hasExplicitReturnType(hasExplicitReturnType)
        {
            if (!this->parameters.empty())
            {
                setLocation(this->parameters[0].name.location);
            }
            else if (this->body != nullptr)
            {
                setLocation(this->body->getLocation());
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::LambdaExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Range expressions (0..10, 0..=10, ..10, 10.., ..=end)
    class RangeExpression final : public Expression
    {
    public:
        UniquePtr<Expression> start; ///< nullptr if no start (RangeTo, RangeToInclusive)
        UniquePtr<Expression> end;   ///< nullptr if no end (RangeFrom)
        bool hasStart;               ///< true if start is present
        bool hasEnd;                 ///< true if end is present
        bool isInclusive;            ///< true for ..=, false for ..

        RangeExpression(
            UniquePtr<Expression> start,
            UniquePtr<Expression> end,
            SourceLocation rangeOperatorLocation,
            bool hasStart,
            bool hasEnd,
            bool isInclusive)
            : start(std::move(start))
            , end(std::move(end))
            , hasStart(hasStart)
            , hasEnd(hasEnd)
            , isInclusive(isInclusive)
        {
            setLocation(rangeOperatorLocation);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::RangeExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Type cast expressions (expr as Type, expr as? Type, expr as! Type)
    class CastExpression final : public Expression
    {
    public:
        enum class CastKind
        {
            Normal,   ///< as - inheritance cast (downcast fails at runtime)
            Optional, ///< as? - optional downcast (Option<T>)
            Forced    ///< as! - forced downcast (panic on failure)
        };

        UniquePtr<Expression> expression;
        UniquePtr<TypeExpression> targetType;
        CastKind kind;
        lexer::Token asToken;

        CastExpression(
            UniquePtr<Expression> expr,
            lexer::Token asToken,
            CastKind castKind,
            UniquePtr<TypeExpression> targetTypeExpr)
            : expression(std::move(expr))
            , targetType(std::move(targetTypeExpr))
            , kind(castKind)
            , asToken(std::move(asToken))
        {
            setLocation(this->asToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::CastExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Type check expressions (expr is Type)
    class TypeCheckExpression final : public Expression
    {
    public:
        UniquePtr<Expression> expression;
        UniquePtr<TypeExpression> targetType;
        lexer::Token isToken;

        TypeCheckExpression(
            UniquePtr<Expression> expr,
            lexer::Token isToken,
            UniquePtr<TypeExpression> targetTypeExpr)
            : expression(std::move(expr))
            , targetType(std::move(targetTypeExpr))
            , isToken(std::move(isToken))
        {
            setLocation(this->isToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TypeCheckExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Try expression (`try`, `try?`, `try!`)
    class TryExpression final : public Expression
    {
    public:
        enum class TryKind
        {
            Propagating, ///< try - propagates errors in a matching throws function
            Optional,    ///< try? - converts Result<T,E> to Option<T>
            Forced       ///< try! - unwraps or panics
        };

        UniquePtr<Expression> expression;
        TryKind kind;
        lexer::Token tryToken; ///< The 'try' keyword token

        TryExpression(
            lexer::Token tryKeyword,
            TryKind tryKind,
            UniquePtr<Expression> expr)
            : expression(std::move(expr))
            , kind(tryKind)
            , tryToken(std::move(tryKeyword))
        {
            setLocation(this->tryToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TryExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Await expression (`await expr`)
    class AwaitExpression final : public Expression
    {
    public:
        UniquePtr<Expression> expression;
        lexer::Token awaitToken;

        AwaitExpression(lexer::Token awaitKeyword, UniquePtr<Expression> expr)
            : expression(std::move(expr))
            , awaitToken(std::move(awaitKeyword))
        {
            setLocation(this->awaitToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::AwaitExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Expression statements
    class ExpressionStatement final : public Statement
    {
    public:
        UniquePtr<Expression> expression;

        explicit ExpressionStatement(UniquePtr<Expression> expression)
            : expression(std::move(expression))
        {
            if (this->expression != nullptr)
            {
                setLocation(this->expression->getLocation());
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ExpressionStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Block statements
    class BlockStatement final : public Statement
    {
    public:
        Array<UniquePtr<Statement>> statements;

        explicit BlockStatement(Array<UniquePtr<Statement>> statements)
            : statements(std::move(statements))
        {
            SourceLocation span;
            for (auto const& statement : this->statements)
            {
                if (statement == nullptr)
                {
                    continue;
                }

                span = span.line == 0 ? statement->getLocation()
                                      : rune::enclosingSpan(span, statement->getLocation());
            }

            if (span.line != 0)
            {
                setLocation(span);
            }
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::BlockStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Variable declarations
    class VariableDeclaration final : public Statement
    {
    public:
        lexer::Token name;                             ///< Single variable name (empty if tuple destructuring)
        Array<lexer::Token> tupleNames;                ///< Names for tuple destructuring (empty if single variable)
        std::optional<UniquePtr<TypeExpression>> type; ///< Optional type annotation
        UniquePtr<Expression> initializer;
        VariableKind variableKind;
        bool isStatic{false}; ///< Whether this declaration is static
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        VariableDeclaration(
            lexer::Token nameToken,
            std::optional<UniquePtr<TypeExpression>> typeExpr,
            UniquePtr<Expression> initializerExpression,
            VariableKind kind,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private,
            bool staticDeclaration = false)
            : name(std::move(nameToken))
            , type(std::move(typeExpr))
            , initializer(std::move(initializerExpression))
            , variableKind(kind)
            , isStatic(staticDeclaration)
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        /// Constructor for tuple destructuring
        VariableDeclaration(
            Array<lexer::Token> tupleNameTokens,
            std::optional<UniquePtr<TypeExpression>> typeExpr,
            UniquePtr<Expression> initializerExpression,
            VariableKind kind,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(lexer::Token{}) ///< Empty token for tuple destructuring
            , tupleNames(std::move(tupleNameTokens))
            , type(std::move(typeExpr))
            , initializer(std::move(initializerExpression))
            , variableKind(kind)
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            if (!tupleNames.empty())
            {
                setLocation(tupleNames[0].location);
            }
        }

        /// Check if this is a tuple destructuring declaration
        bool isTupleDestructuring() const
        {
            return !tupleNames.empty();
        }

        bool isDiscardBinding() const
        {
            return !isTupleDestructuring() && isDiscardToken(name);
        }

        bool isTupleBindingDiscard(usize index) const
        {
            return index < tupleNames.size() && isDiscardToken(tupleNames[index]);
        }

        bool isVar() const
        {
            return variableKind == VariableKind::Var;
        }

        bool isLet() const
        {
            return variableKind == VariableKind::Let;
        }

        bool isConst() const
        {
            return variableKind == VariableKind::Const;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::VariableDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Property accessor (get or set)
    struct PropertyAccessor final
    {
        lexer::Token keyword;                                   ///< "get" or "set"
        std::optional<lexer::Token> parameterName;              ///< Optional parameter name for setter (defaults to "value")
        UniquePtr<Statement> body;                              ///< BlockStatement or ExpressionStatement
        AccessModifier accessModifier{AccessModifier::Private}; ///< Access modifier for this accessor (get/set can have different modifiers)
        Array<UniquePtr<Attribute>> attributes;                 ///< Attributes for this accessor (e.g., @builtin)

        PropertyAccessor(lexer::Token keywordToken, UniquePtr<Statement> bodyStatement, std::optional<lexer::Token> paramName = std::nullopt, AccessModifier access = AccessModifier::Private, Array<UniquePtr<Attribute>> attrs = {})
            : keyword(std::move(keywordToken))
            , parameterName(std::move(paramName))
            , body(std::move(bodyStatement))
            , accessModifier(access)
            , attributes(std::move(attrs))
        {
        }
    };

    /// Property declarations (properties have get/set accessors and do NOT use var/let)
    class PropertyDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        UniquePtr<TypeExpression> type;
        std::optional<UniquePtr<PropertyAccessor>> getter; ///< Optional get accessor
        std::optional<UniquePtr<PropertyAccessor>> setter; ///< Optional set accessor
        UniquePtr<Expression> initializer;                 ///< For auto-properties with default value
        bool isAutoProperty;                               ///< True if using { get, set } syntax (auto-property)
        bool isLiteralSuffix;                              ///< True if declared with 'literal' in a numeric extension
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        PropertyDeclaration(
            lexer::Token nameToken,
            UniquePtr<TypeExpression> typeExpr,
            std::optional<UniquePtr<PropertyAccessor>> getAccessor = std::nullopt,
            std::optional<UniquePtr<PropertyAccessor>> setAccessor = std::nullopt,
            UniquePtr<Expression> initializerExpr = nullptr,
            bool autoProperty = false,
            bool literalSuffix = false,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(nameToken))
            , type(std::move(typeExpr))
            , getter(std::move(getAccessor))
            , setter(std::move(setAccessor))
            , initializer(std::move(initializerExpr))
            , isAutoProperty(autoProperty)
            , isLiteralSuffix(literalSuffix)
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        bool hasGetter() const
        {
            return getter.has_value();
        }
        bool hasSetter() const
        {
            return setter.has_value();
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::PropertyDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// If statements
    class IfStatement final : public Statement
    {
    public:
        UniquePtr<VariableDeclaration> optionalBinding;
        UniquePtr<Expression> condition;
        UniquePtr<Statement> thenBranch;
        UniquePtr<Statement> elseBranch;

        IfStatement(
            UniquePtr<Expression> conditionExpression,
            UniquePtr<Statement> thenStatement,
            UniquePtr<Statement> elseStatement = nullptr)
            : condition(std::move(conditionExpression))
            , thenBranch(std::move(thenStatement))
            , elseBranch(std::move(elseStatement))
        {
        }

        IfStatement(
            UniquePtr<VariableDeclaration> optionalBindingDeclaration,
            UniquePtr<Statement> thenStatement,
            UniquePtr<Statement> elseStatement = nullptr)
            : optionalBinding(std::move(optionalBindingDeclaration))
            , thenBranch(std::move(thenStatement))
            , elseBranch(std::move(elseStatement))
        {
        }

        bool hasOptionalBinding() const
        {
            return optionalBinding != nullptr;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::IfStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Guard statements
    class GuardStatement final : public Statement
    {
    public:
        UniquePtr<VariableDeclaration> optionalBinding;
        UniquePtr<Expression> condition;
        UniquePtr<Statement> elseBranch;

        GuardStatement(
            UniquePtr<Expression> conditionExpression,
            UniquePtr<Statement> elseStatement)
            : condition(std::move(conditionExpression))
            , elseBranch(std::move(elseStatement))
        {
        }

        GuardStatement(
            UniquePtr<VariableDeclaration> optionalBindingDeclaration,
            UniquePtr<Statement> elseStatement)
            : optionalBinding(std::move(optionalBindingDeclaration))
            , elseBranch(std::move(elseStatement))
        {
        }

        bool hasOptionalBinding() const
        {
            return optionalBinding != nullptr;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::GuardStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// While statements
    class WhileStatement final : public Statement
    {
    public:
        UniquePtr<Expression> condition;
        UniquePtr<Statement> body;

        WhileStatement(UniquePtr<Expression> conditionExpression, UniquePtr<Statement> bodyStatement)
            : condition(std::move(conditionExpression))
            , body(std::move(bodyStatement))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::WhileStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Loop statements (loop { ... })
    class LoopStatement final : public Statement
    {
    public:
        UniquePtr<Statement> body;

        explicit LoopStatement(UniquePtr<Statement> bodyStatement)
            : body(std::move(bodyStatement))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::LoopStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// For-in statements (for item in range)
    class ForStatement final : public Statement
    {
    public:
        lexer::Token itemName;       ///< The loop variable
        UniquePtr<Expression> range; ///< The range or collection to iterate over
        UniquePtr<Expression> step;  ///< Optional stride expression (`by` clause)
        UniquePtr<Statement> body;

        ForStatement(
            lexer::Token itemName,
            UniquePtr<Expression> range,
            UniquePtr<Expression> step,
            UniquePtr<Statement> body)
            : itemName(std::move(itemName))
            , range(std::move(range))
            , step(std::move(step))
            , body(std::move(body))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ForStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Break statement
    class BreakStatement final : public Statement
    {
    public:
        lexer::Token keyword;

        explicit BreakStatement(lexer::Token keywordToken)
            : keyword(std::move(keywordToken))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::BreakStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Continue statement
    class ContinueStatement final : public Statement
    {
    public:
        lexer::Token keyword;

        explicit ContinueStatement(lexer::Token keywordToken)
            : keyword(std::move(keywordToken))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ContinueStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Return statements
    class ReturnStatement final : public Statement
    {
    public:
        lexer::Token keyword;
        UniquePtr<Expression> value;

        ReturnStatement(lexer::Token keywordToken, UniquePtr<Expression> valueExpression = nullptr)
            : keyword(std::move(keywordToken))
            , value(std::move(valueExpression))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ReturnStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Throw statements
    class ThrowStatement final : public Statement
    {
    public:
        lexer::Token keyword;
        UniquePtr<Expression> value; ///< Optional error value to throw

        ThrowStatement(lexer::Token keywordToken, UniquePtr<Expression> valueExpression = nullptr)
            : keyword(std::move(keywordToken))
            , value(std::move(valueExpression))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ThrowStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Match pattern - represents a pattern in a match case
    /// Can be:
    /// - Enum/Flags case: .caseName or .caseName(binding1, binding2)
    /// - Union case: .caseName or .caseName(binding1, binding2)
    /// - Expression case: expression (e.g., 1, "ok", true)
    /// - Default case: _ (underscore)
    class MatchPattern final : public ASTNode
    {
    public:
        /// Tag type to disambiguate constructors
        struct DefaultCaseTag final
        {
        };
        static constexpr DefaultCaseTag DefaultCase{};

        bool isDefault;                   ///< true for _ pattern
        lexer::Token caseName;            ///< Case name (e.g., "login", "logout")
        Array<lexer::Token> bindings;     ///< Variable bindings for associated values (e.g., ["username"])
        UniquePtr<Expression> expression; ///< Expression pattern for value matching

        MatchPattern(lexer::Token caseNameToken, Array<lexer::Token> bindingTokens)
            : isDefault(false)
            , caseName(std::move(caseNameToken))
            , bindings(std::move(bindingTokens))
            , expression(nullptr)
        {
        }

        /// Constructor for case with no bindings
        explicit MatchPattern(lexer::Token caseNameToken)
            : isDefault(false)
            , caseName(std::move(caseNameToken))
            , bindings()
            , expression(nullptr)
        {
        }

        /// Constructor for default case
        MatchPattern(lexer::Token underscoreToken, DefaultCaseTag)
            : isDefault(true)
            , caseName(std::move(underscoreToken))
            , bindings()
            , expression(nullptr)
        {
        }

        /// Constructor for expression pattern
        explicit MatchPattern(UniquePtr<Expression> expressionPattern)
            : isDefault(false)
            , caseName()
            , bindings()
            , expression(std::move(expressionPattern))
        {
        }

        bool isBindingDiscard(usize index) const
        {
            return index < bindings.size() && isDiscardToken(bindings[index]);
        }

        ASTNodeKind getKind() const override
        {
            /// MatchPattern is not a top-level AST node kind, but we need this for the base class
            return ASTNodeKind::MatchStatement; ///< Use MatchStatement as placeholder
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Match case - represents one case in a match statement
    class MatchCase final
    {
    public:
        UniquePtr<MatchPattern> pattern;
        UniquePtr<Statement> body; ///< The body statement (usually a block)

        MatchCase(UniquePtr<MatchPattern> patternExpr, UniquePtr<Statement> bodyStmt)
            : pattern(std::move(patternExpr))
            , body(std::move(bodyStmt))
        {
        }
    };

    /// Match statement (pattern matching)
    class MatchStatement final : public Statement
    {
    public:
        UniquePtr<Expression> matchedExpression; ///< The expression being matched
        Array<MatchCase> cases;                  ///< Array of match cases

        MatchStatement(
            UniquePtr<Expression> matchedExpr,
            Array<MatchCase> matchCases)
            : matchedExpression(std::move(matchedExpr))
            , cases(std::move(matchCases))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::MatchStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Catch clause for try-catch blocks
    class CatchClause final
    {
    public:
        UniquePtr<MatchPattern> pattern;          ///< Pattern to match against the error (e.g., .io, .network)
        std::optional<lexer::Token> errorBinding; ///< Optional binding for the error value (e.g., 'error' in 'catch let error')
        UniquePtr<BlockStatement> body;           ///< The catch block body

        /// Constructor for pattern matching catch (catch .caseName)
        CatchClause(UniquePtr<MatchPattern> patternExpr, UniquePtr<BlockStatement> bodyBlock)
            : pattern(std::move(patternExpr))
            , errorBinding(std::nullopt)
            , body(std::move(bodyBlock))
        {
        }

        /// Constructor for error binding catch (catch let error) or catch-all (catch)
        CatchClause(std::optional<lexer::Token> errorBindingToken, UniquePtr<BlockStatement> bodyBlock)
            : pattern(nullptr)
            , errorBinding(std::move(errorBindingToken))
            , body(std::move(bodyBlock))
        {
        }
    };

    /// Try-catch statements
    class TryStatement final : public Statement
    {
    public:
        lexer::Token keyword;               ///< 'try' keyword token
        UniquePtr<BlockStatement> tryBlock; ///< The try block
        Array<CatchClause> catchClauses;    ///< Array of catch clauses

        TryStatement(
            lexer::Token keywordToken,
            UniquePtr<BlockStatement> tryBlockStmt,
            Array<CatchClause> catchClausesArray)
            : keyword(std::move(keywordToken))
            , tryBlock(std::move(tryBlockStmt))
            , catchClauses(std::move(catchClausesArray))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TryStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Make expression: make T(args...), make! T(...), make? T(...), or make(allocator) T(...)
    /// Heap-allocates a value of the given type and returns &T, traps, or Option<&T>.
    class MakeExpression final : public Expression
    {
    public:
        enum class MakeKind
        {
            Panic,    ///< make: trap on allocation failure (default outside try)
            Forced,   ///< make!: explicit trap on failure
            Optional, ///< make?: Option<&T> on failure
        };

        lexer::Token makeToken; ///< The 'make' keyword token
        MakeKind kind{MakeKind::Panic};
        UniquePtr<Expression> allocatorExpr; ///< Optional allocator expression (nullptr if default)
        UniquePtr<TypeExpression> typeName;  ///< The type to allocate
        lexer::Token typeToken;              ///< The token used for the type (for location)
        Array<Argument> arguments;           ///< Constructor arguments

        MakeExpression(
            lexer::Token makeKeyword,
            MakeKind makeKind,
            UniquePtr<Expression> allocator,
            UniquePtr<TypeExpression> type,
            lexer::Token typetok,
            Array<Argument> args)
            : makeToken(std::move(makeKeyword))
            , kind(makeKind)
            , allocatorExpr(std::move(allocator))
            , typeName(std::move(type))
            , typeToken(std::move(typetok))
            , arguments(std::move(args))
        {
            setLocation(makeToken.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::MakeExpression;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Dispose statement: dispose expr or dispose(allocator) expr
    /// Destroys a heap-allocated value and returns its storage to the allocator
    class DisposeStatement final : public Statement
    {
    public:
        lexer::Token keyword;                ///< The 'dispose' keyword token
        UniquePtr<Expression> allocatorExpr; ///< Optional allocator expression (nullptr if default)
        UniquePtr<Expression> value;         ///< The reference to dispose

        DisposeStatement(
            lexer::Token keywordToken,
            UniquePtr<Expression> allocator,
            UniquePtr<Expression> valueExpr)
            : keyword(std::move(keywordToken))
            , allocatorExpr(std::move(allocator))
            , value(std::move(valueExpr))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::DisposeStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Defer statement: defer expr or defer { ... }
    /// Schedules cleanup at scope exit (LIFO within the scope)
    class DeferStatement final : public Statement
    {
    public:
        lexer::Token keyword;
        UniquePtr<Statement> body;

        DeferStatement(lexer::Token keywordToken, UniquePtr<Statement> bodyStatement)
            : keyword(std::move(keywordToken))
            , body(std::move(bodyStatement))
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::DeferStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Function declarations
    class FunctionDeclaration final : public Statement
    {
    public:
        enum class OperatorKind : uint8
        {
            None,
            PrefixUnary,
            PostfixUnary,
            Binary,
            Subscript,
            SubscriptAssignment,
        };

        lexer::Token name;
        Array<TypeParameter> typeParameters; ///< Generic type parameters (e.g., <T, U>)
        Array<Parameter> parameters;
        UniquePtr<TypeExpression> returnType;
        UniquePtr<BlockStatement> body;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};
        bool isStatic{false};                          ///< Whether this is a static method
        bool isMutating{false};                        ///< Whether this is a mutating instance method
        bool isOpen{false};                            ///< Whether this method may be overridden in derived structs
        bool isOverride{false};                        ///< Whether this method overrides an open base method
        bool isAsync{false};                           ///< Whether this function may suspend at await points
        OperatorKind operatorKind{OperatorKind::None}; ///< Operator declaration shape, if this function is an operator.
        UniquePtr<TypeExpression> throwsClause;        ///< Optional error type for throws(E), nullptr if throws with no type

        FunctionDeclaration(
            lexer::Token nameToken,
            Array<TypeParameter> typeParams,
            Array<Parameter> parameters,
            UniquePtr<TypeExpression> returnTypeExpr,
            UniquePtr<BlockStatement> bodyStatement,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private,
            bool mutating = false,
            bool open = false,
            bool overrideMethod = false,
            UniquePtr<TypeExpression> throwsType = nullptr,
            bool staticMethod = false,
            bool asyncFunc = false,
            OperatorKind operatorDeclarationKind = OperatorKind::None)
            : name(std::move(nameToken))
            , typeParameters(std::move(typeParams))
            , parameters(std::move(parameters))
            , returnType(std::move(returnTypeExpr))
            , body(std::move(bodyStatement))
            , attributes(std::move(attributes))
            , accessModifier(access)
            , isStatic(staticMethod)
            , isMutating(mutating)
            , isOpen(open)
            , isOverride(overrideMethod)
            , isAsync(asyncFunc)
            , operatorKind(operatorDeclarationKind)
            , throwsClause(std::move(throwsType))
        {
            SourceLocation span = name.location;
            if (body != nullptr)
            {
                span = rune::enclosingSpan(span, body->getLocation());
            }

            setLocation(span);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::FunctionDeclaration;
        }

        bool isInitializer() const
        {
            return name.type == lexer::TokenType::Init;
        }

        bool isFinalizer() const
        {
            return name.type == lexer::TokenType::Final;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Struct declarations
    class StructDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        Array<TypeParameter> typeParameters;                  ///< Generic type parameters (e.g., <T, U>)
        std::optional<lexer::Token> baseStruct;               ///< Optional base struct this struct extends
        Array<UniquePtr<TypeExpression>> conformedInterfaces; ///< Interfaces this struct conforms to via implements clause
        Array<UniquePtr<VariableDeclaration>> fields;
        Array<UniquePtr<PropertyDeclaration>> properties; ///< Properties (have get/set accessors)
        Array<UniquePtr<FunctionDeclaration>> methods;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};
        bool isOpen{false}; ///< When false (default), the struct cannot be extended

        StructDeclaration(
            lexer::Token name,
            Array<TypeParameter> typeParams,
            std::optional<lexer::Token> baseStruct,
            Array<UniquePtr<TypeExpression>> interfaces,
            Array<UniquePtr<VariableDeclaration>> fields,
            Array<UniquePtr<PropertyDeclaration>> properties,
            Array<UniquePtr<FunctionDeclaration>> methods,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private,
            bool isOpen = false)
            : name(std::move(name))
            , typeParameters(std::move(typeParams))
            , baseStruct(std::move(baseStruct))
            , conformedInterfaces(std::move(interfaces))
            , fields(std::move(fields))
            , properties(std::move(properties))
            , methods(std::move(methods))
            , attributes(std::move(attributes))
            , accessModifier(access)
            , isOpen(isOpen)
        {
            setLocation(this->name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::StructDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Type alias declarations (alias NewName = ExistingType)
    class AliasDeclaration final : public Statement
    {
    public:
        lexer::Token name;                    ///< The new alias name
        UniquePtr<TypeExpression> targetType; ///< The type being aliased
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        AliasDeclaration(
            lexer::Token aliasName,
            UniquePtr<TypeExpression> type,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(aliasName))
            , targetType(std::move(type))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::AliasDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Associated type requirement or witness inside interfaces/extensions.
    class TypeMemberDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        UniquePtr<TypeExpression> targetType;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        TypeMemberDeclaration(
            lexer::Token typeName,
            UniquePtr<TypeExpression> target = nullptr,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(typeName))
            , targetType(std::move(target))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::TypeMemberDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Distinct type declarations (distinct NewName = ExistingType)
    /// Creates a new type that is NOT compatible with the base type
    class DistinctTypeDeclaration final : public Statement
    {
    public:
        lexer::Token name;                  ///< The new type name
        UniquePtr<TypeExpression> baseType; ///< The underlying type
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        DistinctTypeDeclaration(
            lexer::Token typeName,
            UniquePtr<TypeExpression> base = nullptr,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(typeName))
            , baseType(std::move(base))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::DistinctTypeDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Extension declarations (extension TypeName { methods... })
    /// Allows adding methods to existing types (retroactive modeling)
    class ExtensionDeclaration final : public Statement
    {
    public:
        UniquePtr<TypeExpression> targetType; ///< Type being extended
        Array<UniquePtr<FunctionDeclaration>> methods;
        Array<UniquePtr<PropertyDeclaration>> properties; ///< Properties (instance extensions only)
        Array<UniquePtr<VariableDeclaration>> fields;     ///< Const fields (always static members)
        Array<UniquePtr<TypeMemberDeclaration>> typeMembers;
        Array<UniquePtr<Attribute>> attributes;
        Array<UniquePtr<TypeExpression>> conformedInterfaces; ///< Interfaces declared via implements clause
        Array<TypeParameter> typeParameters;                  ///< Generic parameters introduced by constrained target args
        AccessModifier accessModifier{AccessModifier::Private};

        ExtensionDeclaration(
            UniquePtr<TypeExpression> type,
            Array<UniquePtr<FunctionDeclaration>> extensionMethods,
            Array<UniquePtr<Attribute>> attributes = {},
            Array<UniquePtr<VariableDeclaration>> extensionFields = {},
            Array<UniquePtr<PropertyDeclaration>> extensionProperties = {},
            AccessModifier access = AccessModifier::Private,
            Array<UniquePtr<TypeExpression>> conformedInterfaces = {},
            Array<UniquePtr<TypeMemberDeclaration>> typeMembers = {},
            Array<TypeParameter> typeParams = {})
            : targetType(std::move(type))
            , methods(std::move(extensionMethods))
            , properties(std::move(extensionProperties))
            , fields(std::move(extensionFields))
            , typeMembers(std::move(typeMembers))
            , attributes(std::move(attributes))
            , conformedInterfaces(std::move(conformedInterfaces))
            , typeParameters(std::move(typeParams))
            , accessModifier(access)
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ExtensionDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Static scope declarations (static ScopeName { const... func... })
    /// Creates a named scope for organizing constants and functions
    class StaticScopeDeclaration final : public Statement
    {
    public:
        lexer::Token name;                        ///< Scope name (e.g., "Math")
        Array<UniquePtr<Statement>> declarations; ///< Any declarations (like a namespace)
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        StaticScopeDeclaration(
            lexer::Token scopeName,
            Array<UniquePtr<Statement>> scopeDeclarations = {},
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(scopeName))
            , declarations(std::move(scopeDeclarations))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::StaticScopeDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Attribute-scoped declaration groups (`@link(...) { ... }`).
    class AttributedDeclarationGroup final : public Statement
    {
    public:
        Array<UniquePtr<Statement>> declarations;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        AttributedDeclarationGroup(
            Array<UniquePtr<Statement>> declarations,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier accessModifier = AccessModifier::Private)
            : declarations(std::move(declarations))
            , attributes(std::move(attributes))
            , accessModifier(accessModifier)
        {
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::AttributedDeclarationGroup;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Import statement: import A  /  import A.*  /  import A.B  /  import A.B.*
    /// path holds each dot-separated identifier (at least one).
    /// wildcard is true when the statement ends with .* .
    class ImportStatement final : public Statement
    {
    public:
        Array<lexer::Token> path;
        bool wildcard{false};

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::ImportStatement;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Interface declarations (interface InterfaceName { func requirements... })
    /// Defines a contract that types must conform to
    class InterfaceDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        Array<TypeParameter> typeParameters;                 ///< Generic type parameters (e.g., <T, U>)
        Array<UniquePtr<TypeExpression>> extendedInterfaces; ///< Parent interfaces this interface extends
        Array<UniquePtr<TypeMemberDeclaration>> typeMembers; ///< Associated type requirements
        Array<UniquePtr<FunctionDeclaration>> requirements;  ///< Required methods (no bodies)
        Array<UniquePtr<PropertyDeclaration>> properties;    ///< Required properties (get/set only, no bodies)
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        InterfaceDeclaration(
            lexer::Token interfaceName,
            Array<TypeParameter> typeParams,
            Array<UniquePtr<TypeExpression>> parentInterfaces,
            Array<UniquePtr<TypeMemberDeclaration>> associatedTypes,
            Array<UniquePtr<FunctionDeclaration>> methodRequirements,
            Array<UniquePtr<PropertyDeclaration>> propertyRequirements = {},
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(interfaceName))
            , typeParameters(std::move(typeParams))
            , extendedInterfaces(std::move(parentInterfaces))
            , typeMembers(std::move(associatedTypes))
            , requirements(std::move(methodRequirements))
            , properties(std::move(propertyRequirements))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::InterfaceDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Enum case (for enum and flags)
    struct EnumCase final
    {
        lexer::Token name;
        UniquePtr<Expression> value; ///< Optional explicit value

        EnumCase(lexer::Token nameToken, UniquePtr<Expression> valueExpr = nullptr)
            : name(std::move(nameToken))
            , value(std::move(valueExpr))
        {
        }
    };

    /// Union case (for union types)
    struct UnionCase final
    {
        lexer::Token name;
        Array<Parameter> associatedValues; ///< Optional associated values (e.g., playing(level: Int32, score: Int32))

        UnionCase(lexer::Token nameToken, Array<Parameter> values = {})
            : name(std::move(nameToken))
            , associatedValues(std::move(values))
        {
        }
    };

    /// Enum declarations (enum Color: UInt32 { red = 0xFF0000, ... })
    class EnumDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        UniquePtr<TypeExpression> rawType; ///< Required raw type (e.g., UInt32, Int32, String)
        Array<EnumCase> cases;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        EnumDeclaration(
            lexer::Token enumName,
            UniquePtr<TypeExpression> rawTypeExpr,
            Array<EnumCase> enumCases,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(enumName))
            , rawType(std::move(rawTypeExpr))
            , cases(std::move(enumCases))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::EnumDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Union declarations (union Result<T, E> { case ok(T), case error(E) })
    class UnionDeclaration final : public Statement
    {
    public:
        lexer::Token name;
        Array<TypeParameter> typeParameters; ///< Generic type parameters (e.g., <T, E>)
        Array<UnionCase> cases;
        Array<UniquePtr<Attribute>> attributes;
        AccessModifier accessModifier{AccessModifier::Private};

        UnionDeclaration(
            lexer::Token unionName,
            Array<TypeParameter> typeParams,
            Array<UnionCase> unionCases,
            Array<UniquePtr<Attribute>> attributes = {},
            AccessModifier access = AccessModifier::Private)
            : name(std::move(unionName))
            , typeParameters(std::move(typeParams))
            , cases(std::move(unionCases))
            , attributes(std::move(attributes))
            , accessModifier(access)
        {
            setLocation(name.location);
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::UnionDeclaration;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Program (root of AST)
    class Program final : public ASTNode
    {
    public:
        Array<UniquePtr<Statement>> declarations;
        FunctionDeclaration* entryPoint{nullptr};
        Array<String> sourceFilePaths;
        Array<SourceFileID> topLevelDeclarationSourceFileIndices;
        Array<Diagnostic> parseDiagnostics;

        explicit Program(
            Array<UniquePtr<Statement>> declarations,
            FunctionDeclaration* entryPoint = nullptr,
            Array<String> sourceFilePaths = {},
            Array<SourceFileID> topLevelDeclarationSourceFileIndices = {},
            Array<Diagnostic> parseDiagnostics = {})
            : declarations(std::move(declarations))
            , entryPoint(entryPoint)
            , sourceFilePaths(std::move(sourceFilePaths))
            , topLevelDeclarationSourceFileIndices(std::move(topLevelDeclarationSourceFileIndices))
            , parseDiagnostics(std::move(parseDiagnostics))
        {
        }

        /// Returns the source file that owns the top-level declaration at @p index.
        SourceFileID sourceFileForTopLevelDeclaration(usize index) const
        {
            if (index < topLevelDeclarationSourceFileIndices.size())
            {
                return topLevelDeclarationSourceFileIndices[index];
            }

            return 0;
        }

        ASTNodeKind getKind() const override
        {
            return ASTNodeKind::Program;
        }

        void accept(ASTVisitor& visitor) override;
    };

    /// Double-dispatch visitor interface for AST nodes.
    class ASTVisitor
    {
    public:
        virtual ~ASTVisitor() = default;

        virtual void visitAttribute(Attribute& attribute) = 0;

        virtual void visitLiteralExpression(LiteralExpression& expression) = 0;
        virtual void visitVariableExpression(VariableExpression& expression) = 0;
        virtual void visitBinaryExpression(BinaryExpression& expression) = 0;
        virtual void visitUnaryExpression(UnaryExpression& expression) = 0;
        virtual void visitCallExpression(CallExpression& expression) = 0;
        virtual void visitMemberExpression(MemberExpression& expression) = 0;
        virtual void visitOptionalMemberExpression(OptionalMemberExpression& expression) = 0;
        virtual void visitAssignExpression(AssignExpression& expression) = 0;
        virtual void visitArrayLiteralExpression(ArrayLiteralExpression& expression) = 0;
        virtual void visitIndexExpression(IndexExpression& expression) = 0;
        virtual void visitTernaryExpression(TernaryExpression& expression) = 0;
        virtual void visitNilCoalescingExpression(NilCoalescingExpression& expression) = 0;
        virtual void visitIfExpression(IfExpression& expression) = 0;
        virtual void visitMatchExpression(MatchExpression& expression) = 0;
        virtual void visitLambdaExpression(LambdaExpression& expression) = 0;
        virtual void visitRangeExpression(RangeExpression& expression) = 0;
        virtual void visitTypeExpression(TypeExpression& expression) = 0;
        virtual void visitCastExpression(CastExpression& expression) = 0;
        virtual void visitTypeCheckExpression(TypeCheckExpression& expression) = 0;
        virtual void visitTryExpression(TryExpression& expression) = 0;
        virtual void visitAwaitExpression(AwaitExpression& expression) = 0;

        virtual void visitExpressionStatement(ExpressionStatement& statement) = 0;
        virtual void visitBlockStatement(BlockStatement& statement) = 0;
        virtual void visitVariableDeclaration(VariableDeclaration& statement) = 0;
        virtual void visitIfStatement(IfStatement& statement) = 0;
        virtual void visitGuardStatement(GuardStatement& statement) = 0;
        virtual void visitWhileStatement(WhileStatement& statement) = 0;
        virtual void visitLoopStatement(LoopStatement& statement) = 0;
        virtual void visitForStatement(ForStatement& statement) = 0;
        virtual void visitBreakStatement(BreakStatement& statement) = 0;
        virtual void visitContinueStatement(ContinueStatement& statement) = 0;
        virtual void visitReturnStatement(ReturnStatement& statement) = 0;
        virtual void visitFunctionDeclaration(FunctionDeclaration& statement) = 0;
        virtual void visitStructDeclaration(StructDeclaration& statement) = 0;
        virtual void visitAliasDeclaration(AliasDeclaration& statement) = 0;
        virtual void visitTypeMemberDeclaration(TypeMemberDeclaration& statement) = 0;
        virtual void visitDistinctTypeDeclaration(DistinctTypeDeclaration& statement) = 0;
        virtual void visitExtensionDeclaration(ExtensionDeclaration& statement) = 0;
        virtual void visitStaticScopeDeclaration(StaticScopeDeclaration& statement) = 0;
        virtual void visitAttributedDeclarationGroup(AttributedDeclarationGroup& statement) = 0;
        virtual void visitInterfaceDeclaration(InterfaceDeclaration& statement) = 0;
        virtual void visitImportStatement(ImportStatement& statement) = 0;
        virtual void visitEnumDeclaration(EnumDeclaration& statement) = 0;
        virtual void visitUnionDeclaration(UnionDeclaration& statement) = 0;
        virtual void visitMatchPattern(MatchPattern& pattern) = 0;
        virtual void visitMatchStatement(MatchStatement& statement) = 0;
        virtual void visitPropertyDeclaration(PropertyDeclaration& statement) = 0;
        virtual void visitThrowStatement(ThrowStatement& statement) = 0;
        virtual void visitTryStatement(TryStatement& statement) = 0;
        virtual void visitMakeExpression(MakeExpression& expression) = 0;
        virtual void visitDisposeStatement(DisposeStatement& statement) = 0;
        virtual void visitDeferStatement(DeferStatement& statement) = 0;

        virtual void visitProgram(Program& program) = 0;
    };

#pragma region ast_cast

    namespace Internal
    {
        /// Maps an AST node type to its ASTNodeKind discriminator.
        template <typename T>
        struct ASTNodeKindTraits;

        /// Attribute type specialization.
        template <>
        struct ASTNodeKindTraits<Attribute> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::Attribute;
        };

        /// Expression types
        template <>
        struct ASTNodeKindTraits<LiteralExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::LiteralExpression;
        };

        template <>
        struct ASTNodeKindTraits<VariableExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::VariableExpression;
        };

        template <>
        struct ASTNodeKindTraits<BinaryExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::BinaryExpression;
        };

        template <>
        struct ASTNodeKindTraits<UnaryExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::UnaryExpression;
        };

        template <>
        struct ASTNodeKindTraits<CallExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::CallExpression;
        };

        template <>
        struct ASTNodeKindTraits<MemberExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::MemberExpression;
        };

        template <>
        struct ASTNodeKindTraits<OptionalMemberExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::OptionalMemberExpression;
        };

        template <>
        struct ASTNodeKindTraits<AssignExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::AssignExpression;
        };

        template <>
        struct ASTNodeKindTraits<ArrayLiteralExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ArrayLiteralExpression;
        };

        template <>
        struct ASTNodeKindTraits<IndexExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::IndexExpression;
        };

        template <>
        struct ASTNodeKindTraits<TernaryExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TernaryExpression;
        };

        template <>
        struct ASTNodeKindTraits<NilCoalescingExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::NilCoalescingExpression;
        };

        template <>
        struct ASTNodeKindTraits<IfExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::IfExpression;
        };

        template <>
        struct ASTNodeKindTraits<MatchExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::MatchExpression;
        };

        template <>
        struct ASTNodeKindTraits<TryExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TryExpression;
        };

        template <>
        struct ASTNodeKindTraits<AwaitExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::AwaitExpression;
        };

        template <>
        struct ASTNodeKindTraits<LambdaExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::LambdaExpression;
        };

        template <>
        struct ASTNodeKindTraits<RangeExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::RangeExpression;
        };

        template <>
        struct ASTNodeKindTraits<TypeExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TypeExpression;
        };

        template <>
        struct ASTNodeKindTraits<CastExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::CastExpression;
        };

        template <>
        struct ASTNodeKindTraits<TypeCheckExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TypeCheckExpression;
        };

        /// Statement types
        template <>
        struct ASTNodeKindTraits<ExpressionStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ExpressionStatement;
        };

        template <>
        struct ASTNodeKindTraits<BlockStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::BlockStatement;
        };

        template <>
        struct ASTNodeKindTraits<VariableDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::VariableDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<IfStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::IfStatement;
        };

        template <>
        struct ASTNodeKindTraits<GuardStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::GuardStatement;
        };

        template <>
        struct ASTNodeKindTraits<WhileStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::WhileStatement;
        };

        template <>
        struct ASTNodeKindTraits<LoopStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::LoopStatement;
        };

        template <>
        struct ASTNodeKindTraits<ReturnStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ReturnStatement;
        };

        template <>
        struct ASTNodeKindTraits<FunctionDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::FunctionDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<StructDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::StructDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<AliasDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::AliasDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<TypeMemberDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TypeMemberDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<DistinctTypeDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::DistinctTypeDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<ExtensionDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ExtensionDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<StaticScopeDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::StaticScopeDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<AttributedDeclarationGroup> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::AttributedDeclarationGroup;
        };

        template <>
        struct ASTNodeKindTraits<InterfaceDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::InterfaceDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<ForStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ForStatement;
        };

        template <>
        struct ASTNodeKindTraits<BreakStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::BreakStatement;
        };

        template <>
        struct ASTNodeKindTraits<ContinueStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ContinueStatement;
        };

        template <>
        struct ASTNodeKindTraits<ImportStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ImportStatement;
        };

        template <>
        struct ASTNodeKindTraits<EnumDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::EnumDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<UnionDeclaration> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::UnionDeclaration;
        };

        template <>
        struct ASTNodeKindTraits<MatchStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::MatchStatement;
        };

        template <>
        struct ASTNodeKindTraits<ThrowStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::ThrowStatement;
        };

        template <>
        struct ASTNodeKindTraits<TryStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::TryStatement;
        };

        template <>
        struct ASTNodeKindTraits<MakeExpression> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::MakeExpression;
        };

        template <>
        struct ASTNodeKindTraits<DisposeStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::DisposeStatement;
        };

        template <>
        struct ASTNodeKindTraits<DeferStatement> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::DeferStatement;
        };

        /// Program type
        template <>
        struct ASTNodeKindTraits<Program> final
        {
            static constexpr ASTNodeKind kind = ASTNodeKind::Program;
        };
    }

    /// Casts a non-const AST node pointer to a concrete node type.
    /// @tparam To Target AST node type.
    /// @tparam From Source AST node type.
    /// @param from Node pointer to cast.
    /// @return Cast pointer, or null when the kind does not match.
    template <typename To, typename From>
    inline To* ast_cast(From* from)
    {
        if (!from)
        {
            return nullptr;
        }

        if (from->getKind() == Internal::ASTNodeKindTraits<To>::kind)
        {
            return static_cast<To*>(from);
        }

        return nullptr;
    }

    /// Casts a const AST node pointer to a concrete node type.
    /// @tparam To Target AST node type.
    /// @tparam From Source AST node type.
    /// @param from Node pointer to cast.
    /// @return Cast pointer, or null when the kind does not match.
    template <typename To, typename From>
    inline To const* ast_cast(From const* from)
    {
        if (!from)
            return nullptr;

        if (from->getKind() == Internal::ASTNodeKindTraits<To>::kind)
            return static_cast<To const*>(from);

        return nullptr;
    }

    /// Casts an AST node reference to a concrete node type.
    /// @tparam To Target AST node type.
    /// @tparam From Source AST node type.
    /// @param from Node reference to cast.
    /// @return Cast reference.
    /// @throws std::runtime_error when the kind does not match.
    template <typename To, typename From>
    inline To& ast_cast(From& from)
    {
        if (from.getKind() != Internal::ASTNodeKindTraits<To>::kind)
            throw std::runtime_error("ast_cast failed: invalid node type");

        return static_cast<To&>(from);
    }

    /// Returns true when a node pointer matches the target AST node type.
    /// @tparam To Target AST node type.
    /// @tparam From Source AST node type.
    /// @param from Node pointer to test.
    template <typename To, typename From>
    inline bool ast_is(From* from)
    {
        return from && (from->getKind() == Internal::ASTNodeKindTraits<To>::kind);
    }

    /// Returns true when a node reference matches the target AST node type.
    /// @tparam To Target AST node type.
    /// @tparam From Source AST node type.
    /// @param from Node reference to test.
    template <typename To, typename From>
    inline bool ast_is(From& from)
    {
        return from.getKind() == Internal::ASTNodeKindTraits<To>::kind;
    }

    /// Returns true when expression is an assignable l-value (variable, field, or index of variable).
    /// @param expression Expression to test.
    inline bool isAddressableLValue(Expression const* expression)
    {
        if (expression == nullptr)
        {
            return false;
        }

        if (ast_cast<VariableExpression>(expression) != nullptr)
        {
            return true;
        }

        if (auto const* memberExpression = ast_cast<MemberExpression>(expression))
        {
            return ast_cast<VariableExpression>(memberExpression->object.get()) != nullptr;
        }

        if (auto const* indexExpression = ast_cast<IndexExpression>(expression))
        {
            return ast_cast<VariableExpression>(indexExpression->object.get()) != nullptr;
        }

        return false;
    }

#pragma endregion
}
