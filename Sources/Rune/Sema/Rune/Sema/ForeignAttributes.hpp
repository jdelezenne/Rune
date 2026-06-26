#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Sema/BuiltinAttributes.hpp"
#include "Rune/Sema/ForeignBindingTable.hpp"
#include "Rune/Sema/Intrinsics.hpp"

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/UniquePtr.hpp"

namespace rune::parser
{
    struct Attribute;
}

namespace rune
{
    struct ParsedForeignAttribute final
    {
        ForeignBindingKind kind{ForeignBindingKind::Intrinsic};
        Array<StringView> arguments;
    };

    bool hasForeignBindingAttribute(Array<UniquePtr<parser::Attribute>> const& attributes);

    Optional<ParsedForeignAttribute> findForeignBindingAttribute(
        Array<UniquePtr<parser::Attribute>> const& attributes);

    String resolveForeignAbiSymbol(
        ForeignBindingKind kind,
        Array<StringView> const& attributeArguments,
        StringView memberName,
        String* outError = nullptr);

    String makeModuleForeignRegistryKey(
        StringView moduleName,
        StringView declaringType,
        StringView memberName,
        IntrinsicDeclarationKind kind);
}
