#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/SourceFile.hpp"
#include "Rune/Core/UniquePtr.hpp"
#include "Rune/Parser/AST.hpp"

namespace rune
{
    /// Options for driving the lexing/parsing pipeline.
    struct DriverOptions final
    {
        bool verbose{false}; ///< Enable verbose logging during pipeline execution.
    };

    /// Lex and parse source files into one `Program` (multi-file module merge).
    /// @param fileNames Paths to the source files to parse.
    /// @param options Compiler driver options.
    /// @return The merged Program AST if successful, a SourceError otherwise.
    Result<UniquePtr<parser::Program>> parseSourceFiles(
        Array<String> const& fileNames,
        DriverOptions const& options = {});
}

namespace rune::parser
{
    /// Scans program.declarations for an explicit func main() and sets program.entryPoint.
    /// Does not validate the signature. Must be called before sema for executable modules.
    void findExplicitEntryPoint(parser::Program& program);

    /// Returns true when a top-level statement belongs in the synthesized executable main.
    bool isTopLevelExecutableStatement(Statement const& statement);

    /// Rewrites top-level executable statements into a synthesized `main() -> Int`.
    void synthesizeMainForTopLevelExecutableCode(Program& program);

    /// Propagates a source-file index through an AST subtree (source-file attribution).
    /// @param node The AST node root of the subtree to stamp.
    /// @param index The source file ID/index to apply.
    void stampSourceFileIndex(ASTNode& node, SourceFileID index);

    /// Propagates a source-file index through a Statement subtree.
    /// @param statement The statement to stamp.
    /// @param index The source file ID/index to apply.
    void stampSourceFileIndex(Statement& statement, SourceFileID index);

    /// Propagates a source-file index through an Expression subtree.
    /// @param expression The expression to stamp.
    /// @param index The source file ID/index to apply.
    void stampSourceFileIndex(Expression& expression, SourceFileID index);

    /// Propagates a source-file index through a TypeExpression subtree.
    /// @param typeExpression The type expression to stamp.
    /// @param index The source file ID/index to apply.
    void stampSourceFileIndex(TypeExpression& typeExpression, SourceFileID index);
}
