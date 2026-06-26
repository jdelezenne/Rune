#pragma once

#include "Rune/Core/Enum.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/FfiBindings.hpp"
#include "Rune/Sema/Intrinsics.hpp"
#include "Rune/Sema/SemanticDecl.hpp"

namespace rune
{
    enum class ForeignBindingKind : uint8
    {
        Intrinsic,
        Runtime,
        Symbol,
    };

    enum class ForeignBindingFlag : uint8
    {
        None = 0,
        UsesAbiFunction = 1u << 0,
        IsMutating = 1u << 1,
        KindSymbol = 1u << 2,
        IsModuleExport = 1u << 3,
        KindRuntime = 1u << 4,
        /// Set when the enclosing module declares manifest `includeHeaders`, meaning
        /// a C header (not Rune) already provides this Symbol binding's prototype.
        HasCIncludeHeader = 1u << 5,
    };

    RUNE_DEFINE_ENUM_BITWISE_OPERATORS(ForeignBindingFlag, uint8)

    struct ForeignBinding final
    {
        sema::SemanticDeclID semanticDeclId{sema::invalidSemanticDeclId};
        DeclID declId{0};
        IntrinsicId id{invalidIntrinsicId};
        IdentifierID registryKeyId{0};
        IdentifierID abiSymbolId{0};
        TypeID abiReturnTypeId{0};
        Array<FfiParameterAbi> abiParameters;
        ForeignBindingFlag flags{ForeignBindingFlag::None};
    };

    inline ForeignBindingKind foreignBindingKind(ForeignBinding const& binding)
    {
        if (hasFlag(binding.flags, ForeignBindingFlag::KindSymbol))
        {
            return ForeignBindingKind::Symbol;
        }
        if (hasFlag(binding.flags, ForeignBindingFlag::KindRuntime))
        {
            return ForeignBindingKind::Runtime;
        }

        return ForeignBindingKind::Intrinsic;
    }

    inline bool foreignBindingUsesAbiFunction(ForeignBinding const& binding)
    {
        return hasFlag(binding.flags, ForeignBindingFlag::UsesAbiFunction);
    }

    inline bool foreignBindingIsMutating(ForeignBinding const& binding)
    {
        return hasFlag(binding.flags, ForeignBindingFlag::IsMutating);
    }

    inline bool foreignBindingIsModuleExport(ForeignBinding const& binding)
    {
        return hasFlag(binding.flags, ForeignBindingFlag::IsModuleExport);
    }

    inline bool foreignBindingHasCIncludeHeader(ForeignBinding const& binding)
    {
        return hasFlag(binding.flags, ForeignBindingFlag::HasCIncludeHeader);
    }

    ForeignBinding const* findForeignBinding(Array<ForeignBinding> const& bindings, IntrinsicId id);
    ForeignBinding const* findForeignBindingByDecl(Array<ForeignBinding> const& bindings, DeclID declId);
    ForeignBinding const* findForeignBindingBySemanticDecl(
        Array<ForeignBinding> const& bindings,
        sema::SemanticDeclID semanticDeclId);
    ForeignBinding const* findForeignBindingByRegistryKey(
        Array<ForeignBinding> const& bindings,
        IdentifierTable const& identifiers,
        StringView registryKey);
    ForeignBinding const* findForeignBindingByRegistryKeyPrefix(
        Array<ForeignBinding> const& bindings,
        IdentifierTable const& identifiers,
        StringView registryKeyPrefix);
}
