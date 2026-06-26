#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    using SourceFileID = uint32;

    /// One input `.rune` file in a module compilation.
    struct SourceFile final
    {
        SourceFileID id{0};
        String path;
    };
}
