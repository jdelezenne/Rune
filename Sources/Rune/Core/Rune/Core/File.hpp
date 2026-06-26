#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Expected.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    namespace File
    {
        /// Reads the entire contents of a file.
        /// @param fileName Path to the file.
        /// @return File contents on success, or an error message.
        Expected<String, String> read(StringView fileName);

        /// Writes text content to a file, replacing any existing content.
        /// @param fileName Path to the file.
        /// @param content Text to write.
        /// @return True when the write succeeds.
        bool write(StringView fileName, StringView content);

        /// Reads a file and splits it into lines without trailing newline characters.
        /// @param fileName Path to the file.
        /// @return Lines on success, or an error message.
        Expected<Array<String>, String> readLines(StringView fileName);
    }
}
