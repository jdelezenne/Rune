#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::c
{
    Result<String> emitTranslationUnit(ril::Module const& module, StringView abiHeaderName);
    Result<String> emitAbiHeader(ril::Module const& module, StringView moduleName);
}
