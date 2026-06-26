#pragma once

#include "Rune/Common/VerboseFlags.hpp"

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Assert.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Platform.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Sema/ASTContext.hpp"

#ifndef RUNE_ENABLE_VM_BACKEND
#define RUNE_ENABLE_VM_BACKEND 0
#endif

#ifndef RUNE_ENABLE_C_BACKEND
#define RUNE_ENABLE_C_BACKEND 0
#endif

#ifndef RUNE_ENABLE_LLVM_BACKEND
#define RUNE_ENABLE_LLVM_BACKEND 0
#endif

namespace rune
{
    enum class TargetType
    {
        Executable,
        SharedLibrary,
        StaticLibrary,
    };

    enum class BackendKind
    {
        None,
        VM,
        C,
        LLVM,
    };

    inline Optional<BackendKind> parseBackendKind(StringView name)
    {
#if RUNE_ENABLE_VM_BACKEND
        if (name == "vm")
        {
            return BackendKind::VM;
        }
#endif
#if RUNE_ENABLE_C_BACKEND
        if (name == "c")
        {
            return BackendKind::C;
        }
#endif
#if RUNE_ENABLE_LLVM_BACKEND
        if (name == "llvm")
        {
            return BackendKind::LLVM;
        }
#endif

        return std::nullopt;
    }

    struct CompilerOptions final
    {
        String entryPointName = "main";
        TargetType targetType = TargetType::Executable;
        ModuleDeclKind moduleKind{ModuleDeclKind::Executable};
        bool requireEntryPointValidation = false;
        VerboseFlag verboseFlags{VerboseFlag::None};
        bool printStatistics = false;
        bool generateDebugInfo = false;
        /// When false, `debugAssert` macros are stripped at compile time.
        bool enableDebugAssertions = true;
        bool stripUnusedCode = false;
        /// Directories searched for dependency .runemodule interfaces.
        Array<String> modulePaths;
        /// Extra shared libraries to load at runtime (rune run).
        Array<String> linkLibraries;
        /// Directories searched when resolving native dynamic libraries.
        Array<String> linkDirectories;
        /// When compiling a module artifact, the module name used to qualify exported symbols.
        String analyzingModuleName;
        /// Target platform for this compilation (may differ from the host when cross-building).
        Platform::HostPlatform targetPlatform{Platform::getHostPlatform()};
        /// If non-empty, all dependency artifacts built on-the-fly are written here.
        String artifactDirectory;
    };

    inline bool hasCompilerVerboseFlag(CompilerOptions const& options, VerboseFlag flag)
    {
        return hasVerboseFlag(options.verboseFlags, flag);
    }

    inline StringView nativeExecutableSuffix(Platform::HostPlatform platform)
    {
        switch (platform)
        {
            case Platform::HostPlatform::Darwin:
            case Platform::HostPlatform::Linux:
                return "";
            case Platform::HostPlatform::Windows:
                return ".exe";
        }
        RUNE_UNREACHABLE();
    }
}
