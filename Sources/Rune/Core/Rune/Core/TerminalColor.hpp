#pragma once

#include "Rune/Core/StringView.hpp"

namespace rune::terminal
{
    // Returns true when stdout is a TTY and NO_COLOR is not set.
    bool supportsColor();

    StringView reset();
    StringView bold();
    StringView dim();
    StringView cyan();
    StringView yellow();
    StringView green();
    StringView red();
}
