#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Primitives.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/DeclContext.hpp"

namespace rune::ril
{
    BoundDecl const* findRegisteredDecl(ASTContext const& context, parser::Statement const& declaration);
    Result<Type> functionReturnType(ASTContext const& context, parser::FunctionDeclaration const& function);
    Result<TypeID> functionReturnTypeId(ASTContext const& context, parser::FunctionDeclaration const& function);
    Result<Array<Type>> functionParameterTypes(ASTContext const& context, parser::FunctionDeclaration const& function);
}
