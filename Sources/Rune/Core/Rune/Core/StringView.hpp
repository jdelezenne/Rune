#pragma once

#include "Rune/Core/Types.hpp"

#include <string_view>

namespace rune
{
    /// Non-owning view over a UTF-8 string buffer.
    using StringView = std::string_view;
}
