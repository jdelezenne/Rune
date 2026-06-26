#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Modules/Package/Module.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::package
{
    /// Read a `.runemodule` artifact into ASTContext.
    Result<::rune::Module> readPackageIntoContext(ASTContext& context, StringView moduleName);

    Result<Module> readPackageModuleFromBytes(String const& artifactPath, Array<uint8> const& bytes);

    Result<Module> readPackageModuleFromPath(String const& artifactPath);

    Result<::rune::Module> readPackageIntoContextFromBytes(
        ASTContext& context,
        String const& artifactPath,
        Array<uint8> const& bytes);

    Result<::rune::Module> readPackageIntoContextFromPath(ASTContext& context, String const& artifactPath);

}
