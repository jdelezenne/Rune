#pragma once

#include "Rune/Common/BackendInterface.hpp"
#include "Rune/Common/CompilerCommon.hpp"

namespace rune
{
    BackendInterface* getBackend(BackendKind kind);
}
