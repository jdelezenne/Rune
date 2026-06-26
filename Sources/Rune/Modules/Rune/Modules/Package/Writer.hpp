#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Modules/Package/Module.hpp"
#include "Rune/Parser/AST.hpp"

namespace rune::package
{
    Result<Array<uint8>> encodePackage(PackageBuildOptions const& options, parser::Program const& program);
    Result<bool> writePackage(PackageBuildOptions const& options, parser::Program const& program);
}
