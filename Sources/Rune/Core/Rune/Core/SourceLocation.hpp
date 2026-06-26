#pragma once

#include "Rune/Core/Assert.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    /// Source code location or span in a file. Line and column values are 1-based.
    /// Column counts Unicode scalars (UTF-8 codepoints) as the lexer does, not bytes or UTF-16 units.
    struct SourceLocation final
    {
        /// Name of the source file.
        String fileName;

        /// Start line.
        uint32 line = 0;

        /// Start column.
        uint32 column = 0;

        /// End line.
        uint32 endLine = 0;

        /// End column.
        uint32 endColumn = 0;

        /// Constructs an empty source location.
        SourceLocation() = default;

        /// Constructs a single-position source location.
        /// @param fileName Source file name.
        /// @param line Line.
        /// @param column Column.
        SourceLocation(String fileName, uint32 line, uint32 column)
            : SourceLocation(std::move(fileName), line, column, line, column)
        {
        }

        /// Constructs a source span from start to end positions.
        /// @param fileName Source file name.
        /// @param line Start line.
        /// @param column Start column.
        /// @param endLine End line.
        /// @param endColumn End column.
        SourceLocation(String fileName, uint32 line, uint32 column, uint32 endLine, uint32 endColumn)
            : fileName(std::move(fileName))
            , line(line)
            , column(column)
            , endLine(endLine)
            , endColumn(endColumn)
        {
        }

        /// Returns the span length in columns.
        /// @return Number of columns between start and end.
        /// Valid for single-line spans only.
        uint32 getLength() const
        {
            RUNE_ASSERT(isSingleLine());

            return endColumn - column;
        }

        /// Returns true when start and end coordinates are identical.
        bool isSinglePosition() const
        {
            return line == endLine && column == endColumn;
        }

        /// Returns true when start and end coordinates differ.
        bool isMultiPosition() const
        {
            return !isSinglePosition();
        }

        /// Returns true when start and end lines are identical.
        bool isSingleLine() const
        {
            return line == endLine;
        }

        /// Returns true when start and end lines differ.
        bool isMultiLine() const
        {
            return !isSingleLine();
        }
    };

    inline uint32 effectiveEndLine(SourceLocation const& location)
    {
        return location.endLine != 0 ? location.endLine : location.line;
    }

    inline uint32 effectiveEndColumn(SourceLocation const& location)
    {
        return location.endLine != 0 ? location.endColumn : location.column;
    }

    /// Returns the smallest source span enclosing @p left and @p right.
    inline SourceLocation enclosingSpan(SourceLocation const& left, SourceLocation const& right)
    {
        if (left.line == 0)
        {
            return right;
        }

        if (right.line == 0)
        {
            return left;
        }

        SourceLocation span = left;
        if (right.line < span.line || (right.line == span.line && right.column < span.column))
        {
            span.line = right.line;
            span.column = right.column;
        }

        uint32 const leftEndLine = effectiveEndLine(left);
        uint32 const leftEndColumn = effectiveEndColumn(left);
        uint32 const rightEndLine = effectiveEndLine(right);
        uint32 const rightEndColumn = effectiveEndColumn(right);

        if (rightEndLine > leftEndLine ||
            (rightEndLine == leftEndLine && rightEndColumn > leftEndColumn))
        {
            span.endLine = rightEndLine;
            span.endColumn = rightEndColumn;
        }
        else
        {
            span.endLine = leftEndLine;
            span.endColumn = leftEndColumn;
        }

        return span;
    }
}