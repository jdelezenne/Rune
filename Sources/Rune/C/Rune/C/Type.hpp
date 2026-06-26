#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/RIL/Primitives.hpp"

namespace rune::c
{
    Result<String> cTypeName(ril::Type const& type);
    Result<String> emitTypeDeclarations(ril::Module const& module);
}
