#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/RIL/Primitives.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::backend
{
    Result<artifact::TypeIndex> lowerType(
        artifact::Module& module,
        ril::Type const& type);

    Result<artifact::LayoutIndex> lowerLayout(
        artifact::Module& module,
        ril::Type const& type);
}
