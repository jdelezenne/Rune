#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/SourceLocation.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/DeclContext.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune
{
    enum class FfiMarshalKind : uint8
    {
        Value,
        IndirectValue,
        BorrowedPointer,
        MutableLValue,
        HandleOutSlot,
        CStringOutSlot,
        OwnedCStringBorrow,
        CallbackAdapter,
    };

    struct FfiArgumentBinding final
    {
        FfiMarshalKind kind{FfiMarshalKind::Value};
        BoundDecl const* localDecl{nullptr};
        TypeID localTypeId{};
    };

    struct FfiParameterAbi final
    {
        FfiMarshalKind kind{FfiMarshalKind::Value};
        TypeID typeId{};
        Optional<TypeID> pointeeTypeId;
        Optional<TypeID> handleOptionTypeId;
    };

    struct FfiCallSiteBindings final
    {
        Array<Optional<FfiArgumentBinding>> arguments;
    };

    String ffiCallSiteKey(parser::CallExpression const& call);
}
