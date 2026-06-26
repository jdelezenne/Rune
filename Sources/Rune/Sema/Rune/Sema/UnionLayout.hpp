#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/ValueStorageLayout.hpp"

namespace rune::sema
{
    struct UnionCaseLayout final
    {
        uint32 arity{0};
        uint32 payloadSize{0};
        uint32 payloadAlignment{1};
        Array<uint32> payloadOffsets;
    };

    struct UnionLayout final
    {
        uint32 tagOffset{0};
        uint32 tagSize{0};
        uint32 tagAlignment{1};
        uint32 payloadOffset{0};
        uint32 totalSize{0};
        uint32 alignment{1};
        Array<UnionCaseLayout> cases;
    };

    Optional<UnionLayout> unionLayoutForOption(
        ASTContext const& context,
        TypeID wrappedTypeId);

    Optional<UnionLayout> unionLayoutForResult(
        ASTContext const& context,
        TypeID successTypeId,
        TypeID errorTypeId);

    Optional<UnionLayout> unionLayoutForUnionDecl(
        ASTContext const& context,
        parser::UnionDeclaration const& unionDecl);

    Optional<UnionLayout> unionLayoutForType(
        ASTContext const& context,
        TypeID typeId);
}
