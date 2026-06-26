#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Primitives.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::ril
{
    Result<Type> rilTypeFromSemanticType(ASTContext const& context, TypeID typeId, SourceLocation location);
    Result<Type> genericBodyRILTypeFromSemanticType(ASTContext const& context, TypeID typeId, SourceLocation location);
    Result<Type> genericBodyRILTypeFromSemanticType(
        ASTContext const& context,
        TypeID typeId,
        Array<IdentifierID> const& genericParameterNameIds,
        SourceLocation location);
    Type referencePointeeType(Type const& type);
    Type rangeElementType(Type const& type);
    Type arrayElementType(Type const& type);
    Type tupleElementType(Type const& type, uint32 index);
    Type aggregateFieldType(Type const& type, uint32 index);
    Result<Type> expressionType(ASTContext const& context, parser::Expression const& expression);
}
