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
    /// Single on-disk package format. Pre-1.0 shipping: keep at 1.0.0 — do not
    /// bump for feature work, and do not add multi-version readers/writers.
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
        SerializedImplementations = 9,
        Introspection = 10,
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
            case ModuleSectionId::SerializedImplementations:
            case ModuleSectionId::Introspection:
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

            case ModuleSectionId::SerializedImplementations:
                return "serialized-implementations";

            case ModuleSectionId::Introspection:
                return "introspection";
        }

        RUNE_UNREACHABLE();
    }

    inline uint32 computeModuleSectionChecksum(uint8 const* contents, usize size)
    {
        return Fnv1a::hash32Bytes(contents, size);
    }

    inline uint32 computeModuleSectionChecksum(Array<uint8> const& contents)
    {
        return computeModuleSectionChecksum(contents.data(), contents.size());
    }

}
