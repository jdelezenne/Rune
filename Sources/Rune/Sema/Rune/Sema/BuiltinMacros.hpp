#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/IdentifierTable.hpp"

namespace rune
{
    class ASTContext;
}

namespace rune::sema
{
    enum class BuiltinMacroKind : uint8
    {
        None,
        SizeOf,
        AlignmentOf,
        StrideOf,
        NameOf,
        QualifiedNameOf,
        TypeIdOf,
        TypeInfoOf,
        EmbedBytes,
        EmbedString,
        Assert,
        DebugAssert,
        Panic,
        Print,
        PrintLn,
        Platform,
    };

    enum class BuiltinMacroArgumentKind
    {
        Type,
        TypeIdentity,
        TypeIntrospection,
        TypeOrStaticMember,
        SourcePath,
        VariadicExpressions,
        BoolWithOptionalMessage,
        OptionalMessage,
        PlatformNames,
    };

    enum class BuiltinMacroResultKind
    {
        Int,
        UInt,
        UInt64,
        String,
        Array,
        Void,
        Never,
        Bool,
        /// Result is the `Introspection.TypeInfo` struct; the checked type comes
        /// from the synthesized construction expression stored on the ASTContext.
        TypeInfo,
    };

    struct BuiltinMacroDescriptor final
    {
        BuiltinMacroKind kind{BuiltinMacroKind::None};
        IdentifierID nameId{0};
        BuiltinMacroArgumentKind argumentKind;
        BuiltinMacroResultKind resultKind;
        bool statementOnly{false};
    };

    void registerBuiltinMacros(ASTContext& context);

    Optional<BuiltinMacroDescriptor> lookupBuiltinMacro(ASTContext const& context, IdentifierID nameId);
}
