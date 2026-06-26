#pragma once

#include "Rune/RIL/BuilderContext.hpp"

namespace rune::ril
{
    Result<uint32> buildNullablePointerRuneValue(
        FunctionBuilder& builder,
        uint32 rawValue,
        TypeID optionTypeId,
        SourceLocation location);

    Optional<ForeignBindingValue> importedForeignBindingValue(
        ASTContext const& context,
        ASTContext const& moduleContext,
        InterfaceDeclReference const& exported);

    // ABI-aware lowering from a Sema TypeID to a RIL Type, matching how
    // @symbol-declared C functions marshal their parameters and return value
    // (e.g. a nullable raw-pointer Optional lowers to a plain pointer rather
    // than the 16-byte tagged Union that rilTypeFromSemanticType would build).
    Result<Type> rilTypeFromCAbiSemanticType(
        ASTContext const& context,
        TypeID typeId,
        SourceLocation location);
}
