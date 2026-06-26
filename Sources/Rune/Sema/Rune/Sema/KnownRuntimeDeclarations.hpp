#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    enum class KnownRuntimeTypeKind : uint8
    {
        SystemAllocator,
    };

    enum class KnownRuntimeRequirementKind : uint8
    {
        AllocatorAllocate,
        AllocatorDeallocate,
    };

    Optional<TypeID> importKnownRuntimeType(
        ASTContext& context,
        KnownRuntimeTypeKind kind);
    Optional<InterfaceDeclReference> importKnownRuntimeRequirement(
        ASTContext& context,
        KnownRuntimeRequirementKind kind);
}
