#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    namespace StringHelper
    {
        /// Removes leading and trailing whitespace from input.
        /// @param input Text to trim.
        /// @return Trimmed copy of input.
        String trim(StringView input);

        /// Collapses consecutive whitespace runs into single spaces.
        /// @param value Text to normalize.
        /// @return Copy of value with collapsed whitespace.
        String collapseWhitespace(StringView value);

        String shellQuote(String const& value);
    }
}
