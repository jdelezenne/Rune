#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/UniquePtr.hpp"

namespace rune::parser
{
    struct Attribute;
}

namespace rune
{
    inline constexpr char const* builtinAttributeName = "builtin";
    inline constexpr char const* flagsAttributeName = "flags";
    inline constexpr char const* intrinsicAttributeName = "intrinsic";
    inline constexpr char const* runtimeAttributeName = "runtime";
    inline constexpr char const* symbolAttributeName = "symbol";

    bool hasAttributeNamed(
        Array<UniquePtr<parser::Attribute>> const& attributes,
        StringView name);

    bool hasStructIntrinsicAttribute(Array<UniquePtr<parser::Attribute>> const& attributes);
}
