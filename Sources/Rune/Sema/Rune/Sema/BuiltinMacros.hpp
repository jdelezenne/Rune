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
        Assert,
        DebugAssert,
        Panic,
        Print,
        PrintLn,
    };

    enum class BuiltinMacroArgumentKind
    {
        Type,
        TypeOrStaticMember,
        VariadicExpressions,
        BoolWithOptionalMessage,
        OptionalMessage,
    };

    enum class BuiltinMacroResultKind
    {
        Int,
        UInt,
        String,
        Void,
        Never,
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
