#pragma once

#include "Rune/Core/DiagnosticEngine.hpp"
#include "Rune/Core/DiagnosticIds.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Lexer/Token.hpp"
#include "Rune/Parser/AST.hpp"

#include "Rune/Core/UniquePtr.hpp"

namespace rune::parser
{
    /// Recursive-descent parser that builds an AST from a token stream.
    class Parser final
    {
    private:
        Array<lexer::Token> tokens;   ///< Full token stream being parsed.
        uint32 current = 0;           ///< Index of the current token.
        DiagnosticEngine diagnostics; ///< Diagnostics collected during parsing and recovery.
        bool declarationOnlyInCurrentGroup = false;

    public:
        /// Constructs a parser over a token stream.
        /// @param tokenStream Tokens produced by the lexer.
        explicit Parser(Array<lexer::Token> tokenStream);

        /// Parses the token stream into a program AST.
        /// @return Parsed program root (possibly partial when recovery recorded errors).
        Result<UniquePtr<Program>> parse();

        /// Parse diagnostics collected during recovery (also copied onto the program).
        DiagnosticEngine const& getDiagnostics() const
        {
            return diagnostics;
        }

    private:
        /// @name Token Management
        /// @{

        /// Returns true when all tokens have been consumed.
        bool isAtEnd() const;

        /// Returns the current token without consuming it.
        lexer::Token const& peek() const;

        /// Returns the token at @p offset from the current cursor without consuming it.
        /// @param offset Number of tokens to look ahead (0 is the current token).
        lexer::Token const& peekAt(uint32 offset) const;

        /// Returns the most recently consumed token.
        lexer::Token const& previous() const;

        /// Consumes and returns the current token.
        lexer::Token advance();

        /// Returns true when the current token has the given type.
        /// @param type Token type to test.
        bool check(lexer::TokenType type) const;

        /// Consumes the current token when it matches type.
        /// @param type Token type to match.
        /// @return True when the token matched and was consumed.
        bool match(lexer::TokenType type);

        /// Consumes the current token when it matches one of types.
        /// @param types Token types to match.
        /// @return True when a token matched and was consumed.
        bool match(std::initializer_list<lexer::TokenType> types);

        /// Returns true when the current token is a binding name (identifier or discard).
        bool checkBindingName() const;

        /// Consumes a binding name or returns an error.
        /// @param message Error message when the token is not a binding name.
        /// @return Consumed binding-name token.
        Result<lexer::Token> consumeBindingName(String const& message);

        /// Returns true when a token kind can be used as an identifier in value/name positions.
        /// @param type Token kind to test.
        /// @return True when the token kind can be interpreted as an identifier name.
        static bool isIdentifierName(lexer::TokenType type);

        /// Returns true when the current token can be used as an identifier in value/name positions.
        bool checkIdentifierName() const;

        /// Consumes an identifier-name token or returns an error.
        /// @param message Error message when the token is not an identifier name.
        /// @return Consumed token normalized to TokenType::Identifier.
        Result<lexer::Token> consumeIdentifierName(String const& message);

        /// Consumes a property name after '.': identifiers or integer literals (tuple .0, .1, ...).
        Result<lexer::Token> consumePropertyNameAfterDot(String const& message);

        /// @}

        /// @name Error Handling
        /// @{

        /// Consumes a token of the expected type or returns an error.
        /// @param type Expected token type.
        /// @param message Error message when the token does not match.
        /// @return Consumed token.
        Result<lexer::Token> consume(lexer::TokenType type, String const& message);

        /// Builds a source error at the current token location.
        /// @param message Error message.
        SourceError error(String const& message, StringView diagnosticId = DiagnosticIds::parserSyntax) const;

        /// Skips tokens until a likely synchronization point after a parse error.
        void synchronize();

        /// Records a recoverable parse error and continues parsing.
        void recordParseError(SourceError const& error);

        /// Skips to a sync point; advances one token if the stream did not move.
        void recoverFromParseError();

        /// Skips tokens until the current struct body ends or the stream is exhausted.
        void recoverUntilStructMemberBoundary();

        /// @}

        /// @name Declarations
        /// @{

        /// Program -> Declaration*
        Result<Array<UniquePtr<Statement>>> program();

        /// Declaration -> StructDecl | FuncDecl | VarDecl | Statement
        Result<UniquePtr<Statement>> declaration();

        /// StructDecl -> "struct" IDENTIFIER "{" (VarDecl | FuncDecl)* "}"
        Result<UniquePtr<StructDeclaration>> structDeclaration();

        /// AliasDecl -> "alias" IDENTIFIER "=" Type
        Result<UniquePtr<AliasDeclaration>> aliasDeclaration();

        /// TypeMemberDecl -> "type" IDENTIFIER ["=" Type]
        Result<UniquePtr<TypeMemberDeclaration>> typeMemberDeclaration(bool requireTargetType);

        /// DistinctTypeDecl -> "distinct" IDENTIFIER "=" Type
        Result<UniquePtr<DistinctTypeDeclaration>> distinctDeclaration();

        /// ExtensionDecl -> "extension" IDENTIFIER "{" FuncDecl* "}"
        Result<UniquePtr<ExtensionDeclaration>> extensionDeclaration();

        /// StaticScopeDecl -> "static" IDENTIFIER "{" (ConstDecl | FuncDecl)* "}"
        Result<UniquePtr<StaticScopeDeclaration>> staticScopeDeclaration();

        /// AttributedGroupDecl -> "{" (Decl)* "}" with attributes applied to each enclosed declaration.
        Result<UniquePtr<AttributedDeclarationGroup>> attributedDeclarationGroup(
            Array<UniquePtr<Attribute>> attributes,
            AccessModifier accessModifier);

        /// Expands an attributed declaration group into flat declarations.
        Result<Void> expandAttributedDeclarationGroup(
            Array<UniquePtr<Statement>>& target,
            UniquePtr<Statement> declaration);

        /// Propagates group-expandable attributes from a static scope onto its children.
        Result<Void> expandStaticScopeGroupAttributes(StaticScopeDeclaration& scope);

        /// InterfaceDecl -> "interface" IDENTIFIER [TypeParams] ["extends" IDENTIFIER ("," IDENTIFIER)*] "{" FuncDecl* "}"
        Result<UniquePtr<InterfaceDeclaration>> interfaceDeclaration();

        /// EnumDecl -> "enum" IDENTIFIER ":" Type "{" EnumCase* "}"
        Result<UniquePtr<EnumDeclaration>> enumDeclaration();

        /// UnionDecl -> "union" IDENTIFIER ["<" TypeParameters ">"] "{" UnionCase* "}"
        Result<UniquePtr<UnionDeclaration>> unionDeclaration();

        /// ImportStmt -> "import" IDENTIFIER ("." IDENTIFIER)* ("." "*")?
        Result<UniquePtr<ImportStatement>> importStatement();

        /// FuncDecl -> ["static"] ["mutating"] "func" IDENTIFIER "(" Parameters? ")" ("->" Type)? Block
        /// @param isMutating True when the function is marked mutating.
        Result<UniquePtr<FunctionDeclaration>> functionDeclaration(bool isMutating = false, bool allowDeclarationOnly = false);

        /// OperatorDecl -> ["mutating"] "operator" OPERATOR "(" Parameters? ")" ("->" Type)? Block
        /// @param isMutating True when the operator is marked mutating.
        /// @param isStatic True when the operator is declared static.
        /// @param allowDeclarationOnly True when the operator is allowed to omit a body.
        Result<UniquePtr<FunctionDeclaration>> operatorDeclaration(bool isMutating = false, bool isStatic = false, bool allowDeclarationOnly = false);

        /// TypeParameters -> "<" TypeParameter ("," TypeParameter)* ">"
        Result<Array<TypeParameter>> typeParameters();

        /// Parameters -> Parameter ("," Parameter)*
        Result<Array<Parameter>> parameters();

        /// Parameter -> IDENTIFIER ":" Type
        Result<Parameter> parameter();

        /// VarDecl -> ("let" | "var" | "const") IDENTIFIER ":" Type ("=" Expression)? ";"?
        Result<UniquePtr<VariableDeclaration>> variableDeclaration();

        /// PropertyDecl -> IDENTIFIER ":" Type "{" (Accessor | AutoProperty) "}"
        /// @param isLiteralSuffix True when parsing a literal suffix property in a numeric extension.
        Result<UniquePtr<PropertyDeclaration>> propertyDeclaration(bool isLiteralSuffix = false);

        /// InterfacePropertyDecl -> IDENTIFIER ":" Type "{" ("get" | "set" | "get" "," "set") "}"
        Result<UniquePtr<PropertyDeclaration>> interfacePropertyDeclaration();

        /// @}

        /// @name Statements
        /// @{

        /// Statement -> ExprStmt | Block | IfStmt | WhileStmt | ReturnStmt | ...
        Result<UniquePtr<Statement>> statement();

        /// Block body entry: nested functions, local bindings, and executable statements.
        Result<UniquePtr<Statement>> blockBodyStatement();

        /// ExprStmt -> Expression ";"?
        Result<UniquePtr<ExpressionStatement>> expressionStatement();

        /// Block -> "{" BlockBodyStatement* "}"
        Result<UniquePtr<BlockStatement>> block();

        /// IfStmt -> "if" Expression Statement ("else" Statement)?
        Result<UniquePtr<IfStatement>> ifStatement();

        /// GuardStmt -> "guard" (Expression | "let" IDENTIFIER "=" Expression) "else" Block
        Result<UniquePtr<GuardStatement>> guardStatement();

        /// WhileStmt -> "while" Expression Statement
        Result<UniquePtr<WhileStatement>> whileStatement();

        /// LoopStmt -> "loop" Statement
        Result<UniquePtr<LoopStatement>> loopStatement();

        /// ForStmt -> "for" IDENTIFIER "in" Expression Statement
        Result<UniquePtr<ForStatement>> forStatement();

        /// BreakStmt -> "break" ";"?
        Result<UniquePtr<BreakStatement>> breakStatement();

        /// ContinueStmt -> "continue" ";"?
        Result<UniquePtr<ContinueStatement>> continueStatement();

        /// ReturnStmt -> "return" Expression? ";"?
        Result<UniquePtr<ReturnStatement>> returnStatement();

        /// ThrowStmt -> "throw" Expression? ";"?
        Result<UniquePtr<ThrowStatement>> throwStatement();

        /// TryStmt -> "try" Block ("catch" MatchPattern? Block)+
        Result<UniquePtr<TryStatement>> tryStatement();

        /// DisposeStmt -> "dispose" ("(" Expression ")")? Expression ";"?
        Result<UniquePtr<DisposeStatement>> disposeStatement();

        /// DeferStmt -> "defer" ( Block | Statement )
        Result<UniquePtr<DeferStatement>> deferStatement();

        /// MatchStmt -> "match" Expression "{" MatchCase* "}"
        Result<UniquePtr<MatchStatement>> matchStatement();

        /// MatchCase -> MatchPattern "=>" Statement
        Result<MatchCase> matchCase();

        /// MatchPattern -> "." IDENTIFIER ("(" IDENTIFIER ("," IDENTIFIER)* ")")? | "_"
        Result<UniquePtr<MatchPattern>> matchPattern();

        /// @}

        /// @name Expressions
        /// @{

        /// Expression -> Assignment
        Result<UniquePtr<Expression>> expression();

        /// Assignment -> IDENTIFIER ("=" | "+=" | ...) Assignment | Ternary
        Result<UniquePtr<Expression>> assignment();

        /// Ternary -> NilCoalescing ("?" Expression ":" Ternary)?
        Result<UniquePtr<Expression>> ternary();

        /// NilCoalescing -> LogicalOr ("??" LogicalOr)*
        Result<UniquePtr<Expression>> nilCoalescing();

        /// LogicalOr -> LogicalAnd ("||" | "or" LogicalAnd)*
        Result<UniquePtr<Expression>> logicalOr();

        /// LogicalAnd -> Pipe ("&&" | "and" Pipe)*
        Result<UniquePtr<Expression>> logicalAnd();

        /// Pipe -> Caret ("|" Caret)*
        Result<UniquePtr<Expression>> bitwiseOr();

        /// Caret -> Ampersand ("^" Ampersand)*
        Result<UniquePtr<Expression>> bitwiseXor();

        /// Ampersand -> Equality ("&" Equality)*
        Result<UniquePtr<Expression>> bitwiseAnd();

        /// Equality -> Comparison (("!=" | "==") Comparison)*
        Result<UniquePtr<Expression>> equality();

        /// Comparison -> Range ("is" Type)? ((">" | ">=" | "<" | "<=" | "<=>") Range)*
        Result<UniquePtr<Expression>> comparison();

        /// Range -> Shift ((".." | "..=") Shift)?
        Result<UniquePtr<Expression>> range();

        /// Shift -> Term (("<<" | ">>") Term)*
        Result<UniquePtr<Expression>> shift();

        /// Term -> Factor (("-" | "+") Factor)*
        Result<UniquePtr<Expression>> term();

        /// Factor -> Unary (("/" | "*" | "%") Unary)*
        Result<UniquePtr<Expression>> factor();

        /// Unary -> ("!" | "not" | "~" | "-") Unary | Cast
        Result<UniquePtr<Expression>> unary();

        /// Cast -> Call ("as" Type)*
        Result<UniquePtr<Expression>> cast();

        /// Call -> Primary ("(" Arguments? ")" | "[" Expression "]" | "." IDENTIFIER)*
        Result<UniquePtr<Expression>> call();

        /// Arguments -> (NamedArgument | PositionalArgument) ("," ...)*
        Result<Array<Argument>> arguments();

        /// Primary -> literal | IDENTIFIER | "(" Expression ")" | ArrayLiteral | Lambda
        Result<UniquePtr<Expression>> primary();

        /// ArrayLiteral -> "[" (Expression ("," Expression)*)? "]"
        Result<UniquePtr<ArrayLiteralExpression>> arrayLiteral();

        /// Lambda -> "(" Parameters? ")" "->" (Type Block | Expression)
        Result<UniquePtr<LambdaExpression>> lambda();

        /// @}

        /// @name Attributes and Types
        /// @{

        /// Attribute -> "@" IDENTIFIER ("(" IDENTIFIER ("," IDENTIFIER)* ")")?
        Result<UniquePtr<Attribute>> attribute();

        /// Parses zero or more consecutive attributes.
        Result<Array<UniquePtr<Attribute>>> attributes();

        /// Parses optional leading attributes followed by an optional access modifier.
        /// @param accessModifier Out parameter receiving the access modifier.
        /// @param attributes Out array receiving the parsed attributes.
        Result<Void> parseLeadingAccessAndAttributes(
            AccessModifier& accessModifier,
            Array<UniquePtr<Attribute>>& attributes,
            bool* hasAccessModifier = nullptr);

        /// Returns true when type is a token that can start a type expression.
        /// @param type Token type to test.
        /// @return True if the token can start a type expression.
        bool isType(lexer::TokenType type) const;

        /// Returns true when the current token can start a statement.
        /// @return True if the current token can start a statement.
        bool isStatement() const;

        /// Returns true when the current token can start an expression.
        /// @return True if the current token can start an expression.
        bool isExpression() const;

        /// Parses a type expression, including generics and optional suffixes.
        /// @param optionalSuffixOnly When true, only parse trailing optional suffixes on an existing type.
        /// @return The parsed TypeExpression AST node.
        Result<UniquePtr<TypeExpression>> parseType(bool optionalSuffixOnly = true);

        /// Parses the primary portion of a type expression.
        /// @return The parsed TypeExpression AST node primary part.
        Result<UniquePtr<TypeExpression>> parseTypePrimary();

        /// Parses an interpolated string into a concatenation expression chain.
        /// @param stringToken String literal token containing interpolation segments.
        Result<UniquePtr<Expression>> parseInterpolatedString(lexer::Token const& stringToken);

        /// Recursively updates an expression subtree to preserve fileName in source locations.
        /// @param expr Expression subtree to update.
        /// @param baseLocation Base source location supplying the file name.
        void updateExpressionLocation(Expression* expr, SourceLocation const& baseLocation);

        /// @}
    };

    /// Serializes a type expression to its source-like string form.
    /// @param typeExpr Type expression to serialize.
    /// @return String representation of the type.
    String typeExpressionToString(TypeExpression const& typeExpr);

    /// Serializes a parameter's type to its source-like string form.
    /// @param parameter Parameter whose type should be serialized.
    /// @return String representation of the parameter type.
    String parameterTypeToString(Parameter const& parameter);
}
