#pragma once

#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune::vm::artifact
{
    inline constexpr StringView signature = "RUNEVMB";

    enum class SectionId : uint32
    {
        Summary = 1,
        Types = 2,
        Constants = 3,
        Layouts = 4,
        Functions = 5,
        Instructions = 6,
        Globals = 7,
        Imports = 8,
        Abi = 9,
        Debug = 10,
        StaticData = 11,
        Introspection = 12,
    };

    enum class ModuleKind : uint8
    {
        Library,
        Executable,
    };

    struct SummaryRecord final
    {
        ModuleKind kind{ModuleKind::Library};
        uint32 moduleIndex{0};
        uint32 entryFunction{0xFFFF'FFFFu};
        uint32 functionCount{0};
        uint32 layoutCount{0};
        uint32 globalCount{0};
        uint32 abiCount{0};
    };
}
