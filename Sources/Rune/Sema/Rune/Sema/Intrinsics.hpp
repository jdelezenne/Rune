#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Fnv1a.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune
{
    class ASTContext;
    struct ForeignBindingValue;

    /// Stable intrinsic operation identifier (FNV-1a of the canonical dotted name).
    using IntrinsicId = uint64;

    inline constexpr IntrinsicId invalidIntrinsicId = 0;

    constexpr IntrinsicId computeIntrinsicId(StringView runeName)
    {
        if (runeName.empty())
        {
            return invalidIntrinsicId;
        }

        return Fnv1a::staticHash(runeName);
    }

    /// Kind of declaration carrying an `@intrinsic` attribute.
    enum class IntrinsicDeclarationKind : uint8
    {
        Function,
        Getter,
        Setter,
        Init,
    };

    enum class CompilerIntrinsicKind : uint8
    {
        PrimitiveCompare,
        PrimitiveEqual,
        TypeSizeOf,
        TypeAlignmentOf,
        TypeStrideOf,
    };

    inline constexpr StringView primitiveCompareIntrinsicName = "compiler.primitive_compare";
    inline constexpr StringView primitiveEqualIntrinsicName = "compiler.primitive_equal";

    /// Builds `Type.member` from a declaring type stem and member (e.g. `Array.append`).
    String makeIntrinsicName(StringView declaringTypeName, StringView memberName, IntrinsicDeclarationKind kind);

    /// Builds the registry rune name for a module-scoped `@intrinsic` declaration.
    /// Root functions use `Module.member`; type members use `Module.Type.member`.
    String makeModuleIntrinsicRuneName(
        StringView moduleName,
        StringView declaringType,
        StringView memberName,
        IntrinsicDeclarationKind kind);

    /// Resolves the canonical intrinsic name directly from parsed `@intrinsic`
    /// usage and declaration context.
    String resolveIntrinsicName(
        StringView attributeName,
        Array<StringView> const& attributeArguments,
        StringView declaringTypeName,
        StringView memberName,
        IntrinsicDeclarationKind kind,
        String* outError = nullptr);

    Optional<CompilerIntrinsicKind> compilerIntrinsicKind(IntrinsicId id);
}
