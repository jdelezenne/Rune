#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Assert.hpp"
#include "Rune/Core/Fnv1a.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Version.hpp"

namespace rune::package
{
    /// Binary `.runemodule` file signature (first bytes on disk).
    inline constexpr StringView packageSignature = "RUNEMOD";
    inline constexpr Version packageFormatVersion = {1, 0, 0};

    enum class ModuleSectionId : uint32
    {
        Module = 1,
        Symbols = 2,
        Attributes = 3,
        Types = 4,
        Conformances = 5,
        Layouts = 6,
        Witnesses = 7,
        Debug = 8,
        GenericImplementations = 9,
    };

    inline bool isKnownModuleSectionId(uint32 sectionId)
    {
        switch (static_cast<ModuleSectionId>(sectionId))
        {
            case ModuleSectionId::Module:
            case ModuleSectionId::Symbols:
            case ModuleSectionId::Attributes:
            case ModuleSectionId::Types:
            case ModuleSectionId::Conformances:
            case ModuleSectionId::Layouts:
            case ModuleSectionId::Witnesses:
            case ModuleSectionId::Debug:
            case ModuleSectionId::GenericImplementations:
                return true;
        }

        return false;
    }

    inline StringView moduleSectionName(ModuleSectionId sectionId)
    {
        switch (sectionId)
        {
            case ModuleSectionId::Module:
                return "module";

            case ModuleSectionId::Symbols:
                return "symbols";

            case ModuleSectionId::Attributes:
                return "attributes";

            case ModuleSectionId::Types:
                return "types";

            case ModuleSectionId::Conformances:
                return "conformances";

            case ModuleSectionId::Layouts:
                return "layouts";

            case ModuleSectionId::Witnesses:
                return "witnesses";

            case ModuleSectionId::Debug:
                return "debug";

            case ModuleSectionId::GenericImplementations:
                return "generic-implementations";
        }

        RUNE_UNREACHABLE();
    }

    inline uint32 computeModuleSectionChecksum(Array<uint8> const& contents)
    {
        return Fnv1a::hash32Bytes(contents);
    }

}
