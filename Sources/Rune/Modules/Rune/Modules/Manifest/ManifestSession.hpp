#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::manifest
{
    ModuleDecl makeModuleDeclFromManifest(ModuleManifest const& manifest);

    Result<ASTContext> makeASTContextFromManifest(ModuleManifest const& manifest);
}
