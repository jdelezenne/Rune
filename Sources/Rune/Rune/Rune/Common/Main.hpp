#pragma once

#include "Rune/Common/CompilerCommon.hpp"
#include "Rune/Common/DevFormat.hpp"
#include "Rune/Modules/InspectModule.hpp"
#include "Rune/Parser/Pipeline.hpp"

#include "Rune/Core/Array.hpp"

namespace rune
{
    bool tokenize(Array<String> const& fileNames, DevOutputOptions const& options = {});
    bool parse(Array<String> const& fileNames, DevOutputOptions const& options = {});
}
