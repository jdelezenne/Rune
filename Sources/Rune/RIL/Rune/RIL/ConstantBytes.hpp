#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Primitives.hpp"

namespace rune::ril
{
    Result<Array<uint8>> constantStorageBytes(
        Constant const& constant,
        uint32 byteCount);
}
