#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    namespace CompilationSession
    {
        String resolveSourceModuleName(
            Array<String> const& sourceFiles,
            String const& moduleName);

        ModuleDecl makeModuleDeclFromSources(
            Array<String> sourceFiles,
            String moduleName,
            ModuleDeclKind kind,
            Platform::HostPlatform targetPlatform = Platform::getHostPlatform());

        Result<ASTContext> makeASTContextFromSources(
            Array<String> sourceFiles,
            String moduleName,
            ModuleDeclKind kind,
            Platform::HostPlatform targetPlatform = Platform::getHostPlatform());
    }
}
