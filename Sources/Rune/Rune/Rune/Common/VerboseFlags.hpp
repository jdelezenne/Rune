#pragma once

#include "Rune/Core/Enum.hpp"
#include "Rune/Core/InlineArray.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    enum class VerboseFlag : uint32
    {
        None = 0,
        Parse = 1u << 0,
        Package = 1u << 1,
        Emit = 1u << 2,
        Run = 1u << 3,
        SemaPipeline = 1u << 4,
        SemaDeclaration = 1u << 5,
        SemaImport = 1u << 6,
        SemaLookup = 1u << 7,
        SemaType = 1u << 8,
        Sema = SemaPipeline | SemaDeclaration | SemaImport | SemaLookup | SemaType,
        All = Parse | Package | Emit | Run | Sema,
    };

    RUNE_DEFINE_ENUM_BITWISE_OPERATORS(VerboseFlag, uint32)

    constexpr bool hasVerboseFlag(VerboseFlag flags, VerboseFlag flag)
    {
        return hasFlag(flags, flag);
    }

    struct VerboseFlagSpec final
    {
        StringView name;
        VerboseFlag flags;
    };

    inline constexpr InlineArray verboseFlagSpecs{
        VerboseFlagSpec{"all", VerboseFlag::All},
        VerboseFlagSpec{"none", VerboseFlag::None},
        VerboseFlagSpec{"parse", VerboseFlag::Parse},
        VerboseFlagSpec{"package", VerboseFlag::Package},
        VerboseFlagSpec{"emit", VerboseFlag::Emit},
        VerboseFlagSpec{"run", VerboseFlag::Run},
        VerboseFlagSpec{"sema", VerboseFlag::Sema},
        VerboseFlagSpec{"sema.pipeline", VerboseFlag::SemaPipeline},
        VerboseFlagSpec{"sema.decl", VerboseFlag::SemaDeclaration},
        VerboseFlagSpec{"sema.import", VerboseFlag::SemaImport},
        VerboseFlagSpec{"sema.lookup", VerboseFlag::SemaLookup},
        VerboseFlagSpec{"sema.type", VerboseFlag::SemaType},
    };

    inline String verboseFlagUsageList()
    {
        String result;
        for (usize index = 0; index < verboseFlagSpecs.size(); ++index)
        {
            if (index != 0)
            {
                result += ',';
            }

            result += verboseFlagSpecs[index].name;
        }

        return result;
    }

    inline bool verboseFlagForName(StringView name, VerboseFlag& out)
    {
        for (VerboseFlagSpec spec : verboseFlagSpecs)
        {
            if (name == spec.name)
            {
                out = spec.flags;
                return true;
            }
        }

        return false;
    }

    inline bool parseVerboseFlags(StringView categories, VerboseFlag& out, String& invalidCategory)
    {
        VerboseFlag parsed = VerboseFlag::None;
        usize start = 0;
        while (start <= categories.size())
        {
            usize const comma = categories.find(',', start);
            usize const end = comma == StringView::npos ? categories.size() : comma;
            StringView const category = categories.substr(start, end - start);

            VerboseFlag flag = VerboseFlag::None;
            if (category.empty() || !verboseFlagForName(category, flag))
            {
                invalidCategory = String(category);
                return false;
            }

            if (flag == VerboseFlag::None)
            {
                parsed = VerboseFlag::None;
            }
            else
            {
                parsed |= flag;
            }

            if (comma == StringView::npos)
            {
                break;
            }
            start = comma + 1;
        }

        out = parsed;
        return true;
    }
}
