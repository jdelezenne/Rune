#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    namespace CommandLine
    {
        // Keep module-interface lookup separate from native dynamic-library lookup:
        //
        // 1. -I / --module-path  Where to find .runemodule dependency interfaces.
        // 2. -L / --link-path    Where to find native dynamic libraries.

        inline constexpr StringView platform = "--platform";

        // rune build
        inline constexpr StringView executable = "--executable";
        inline constexpr StringView library = "--library";
        inline constexpr StringView name = "--name";
        inline constexpr StringView format = "--format";

        // Shared compiler (run, build, compile, transpile, ...)
        inline constexpr StringView entry = "--entry";
        inline constexpr StringView target = "--target";
        inline constexpr StringView debug = "--debug";
        inline constexpr StringView strip = "--strip";
        inline constexpr StringView modulePath = "--module-path";
        inline constexpr StringView modulePathShort = "-I";
        inline constexpr StringView dependency = "--dependency";
        inline constexpr StringView dependencyShort = "-d";

        inline constexpr StringView linkPath = "--link-path";
        inline constexpr StringView linkPathShort = "-L";

        // Common
        inline constexpr StringView output = "--output";
        inline constexpr StringView outputShort = "-o";
        inline constexpr StringView verbose = "--verbose";
        inline constexpr StringView verboseShort = "-v";
        inline constexpr StringView view = "--view";
        inline constexpr StringView function = "--function";
        inline constexpr StringView source = "--source";
        inline constexpr StringView metadata = "--metadata";
        inline constexpr StringView exitCode = "--exitcode";
        inline constexpr StringView optimize = "--optimize";

        inline constexpr StringView bindgenModule = "--module";
    }
}
