#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Core/UniquePtr.hpp"

#include <mutex>

namespace rune
{
    struct SourceLocation;

    /// Centralized registry of source buffers for one compilation.
    ///
    /// Each registered source gets a stable uint32 ID. The manager builds a byte-offset
    /// line table at registration time and uses it to resolve (sourceId, byteOffset) pairs
    /// to (file, line, column) triples on demand, only when formatting a diagnostic.
    ///
    /// StringViews returned by getLexeme() are stable for the lifetime of this SourceManager
    /// because each SourceBuffer is heap-allocated and never relocated after registration.
    class SourceManager final
    {
    public:
        /// Sentinel value meaning "no registered source".
        static constexpr uint32 noSource = UINT32_MAX;

        SourceManager() = default;

        SourceManager(SourceManager const&) = delete;
        SourceManager& operator=(SourceManager const&) = delete;
        SourceManager(SourceManager&& other) noexcept
        {
            std::lock_guard<std::mutex> lock(other.mutex);
            buffers = std::move(other.buffers);
        }

        SourceManager& operator=(SourceManager&& other) noexcept
        {
            if (this != &other)
            {
                std::lock(mutex, other.mutex);
                std::lock_guard<std::mutex> lockThis(mutex, std::adopt_lock);
                std::lock_guard<std::mutex> lockOther(other.mutex, std::adopt_lock);
                buffers = std::move(other.buffers);
            }

            return *this;
        }

        /// Registers a source buffer and returns its stable source ID.
        ///
        /// Builds a byte-offset line table at registration time so that line/column
        /// resolution is a binary search rather than a linear scan.
        ///
        /// Also populates the global SourceContext cache so that existing diagnostic
        /// infrastructure (ErrorHandler, LSP) continues to work during migration.
        ///
        /// @param fileName  Logical name of the source (file path or "<synthetic>").
        /// @param content   Full source text; ownership is transferred to the manager.
        /// @return          Stable source ID valid for the lifetime of this SourceManager.
        uint32 registerFile(String fileName, String content);

        /// Returns the file name associated with @p sourceId.
        StringView getFileName(uint32 sourceId) const;

        /// Returns the full source text of the registered buffer.
        StringView getContent(uint32 sourceId) const;

        /// Returns a view of the lexeme at the given byte range within a source buffer.
        ///
        /// The returned view is stable for the lifetime of this SourceManager.
        StringView getLexeme(uint32 sourceId, uint32 offset, uint32 length) const;

        /// Resolves a byte range in a source buffer to a full SourceLocation.
        ///
        /// Line and column numbers are 1-based. Column counts Unicode codepoints from
        /// the line start (not bytes), matching the existing Lexer convention.
        ///
        /// This is intentionally not cached: call it only when formatting a diagnostic,
        /// not on every token in the happy path.
        ///
        /// @param sourceId     Source ID returned by registerFile.
        /// @param startOffset  Byte offset of the first character of the span.
        /// @param endOffset    Byte offset one past the last character (exclusive end).
        SourceLocation resolve(uint32 sourceId, uint32 startOffset, uint32 endOffset) const;

        /// Returns true when @p sourceId refers to a registered source buffer.
        bool hasSource(uint32 sourceId) const;

        /// Returns the number of registered source buffers.
        uint32 sourceCount() const;

    private:
        struct SourceBuffer
        {
            String fileName;
            String content;
            /// lineTable[i] = byte offset of the first byte of line (i + 1) in content.
            /// lineTable[0] is always 0 (line 1 starts at the beginning of the buffer).
            Array<uint32> lineTable;
        };

        /// Builds a byte-offset line table by scanning for newline characters.
        static Array<uint32> buildLineTable(StringView content);

        /// Counts UTF-8 codepoints in content[from, to).
        static uint32 countCodepoints(StringView content, uint32 from, uint32 to);

        bool hasSourceNoLock(uint32 sourceId) const;

        mutable std::mutex mutex;

        /// Heap-allocated to keep StringViews returned by getLexeme() stable
        /// even as new buffers are registered.
        Array<UniquePtr<SourceBuffer>> buffers;
    };
}
