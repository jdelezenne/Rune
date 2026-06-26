#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/TypeBinding.hpp"

namespace rune::sema
{
    enum class GenericParameterKind : uint8
    {
        Type,
        Value,
    };

    struct GenericParameterInfo final
    {
        GenericParameterKind kind{GenericParameterKind::Type};
        TypeID constraintTypeId{0};
    };

    Optional<GenericParameterInfo> analyzeGenericTypeParameter(
        TypeBindContext const& bindContext,
        parser::TypeParameter const& parameter,
        parser::ASTNode const& errorNode);

    bool isGenericValueParameterName(
        TypeBindContext const& bindContext,
        IdentifierID nameId);

    /// Returns the interface constraint type of the generic type parameter
    /// named `nameId` in the current binding scope, or nullopt when there is no
    /// such parameter or its constraint is not an interface. Used to resolve
    /// interface-requirement member/operator access on a value of constrained
    /// generic type `T extends I`.
    Optional<TypeID> constraintInterfaceTypeForGenericParameterName(
        TypeBindContext const& bindContext,
        IdentifierID nameId,
        parser::ASTNode const& errorNode);

    TypeID resolveGenericValueArgument(
        TypeBindContext const& bindContext,
        parser::TypeExpression const& argument,
        TypeID constraintTypeId,
        parser::ASTNode const& errorNode);

    TypeID resolveGenericInstantiationArgument(
        TypeBindContext const& bindContext,
        parser::TypeParameter const& parameter,
        parser::TypeExpression const& argument,
        parser::ASTNode const& errorNode);

    TypeID resolveInterfaceGenericInstantiationArgument(
        TypeBindContext const& bindContext,
        Module const& module,
        GenericParameterKind kind,
        TypeID interfaceConstraintTypeId,
        parser::TypeExpression const& argument,
        parser::ASTNode const& errorNode);

    /// Validates ordering and constraint fit for type-parameter defaults at the declaration site.
    void validateGenericTypeParameters(
        TypeBindContext const& bindContext,
        Array<parser::TypeParameter> const& parameters,
        parser::ASTNode const& errorNode);

    /// Fills missing generic arguments from explicit args, defaults, or contextual type.
    Optional<Array<TypeID>> resolveGenericInstantiationTypeArguments(
        TypeBindContext const& bindContext,
        Array<parser::TypeParameter> const& parameters,
        Array<UniquePtr<parser::TypeExpression>> const& providedArguments,
        parser::ASTNode const& errorNode,
        Optional<TypeID> contextualInstantiatedType = std::nullopt,
        IdentifierID nominalNameId = 0);

    Optional<Array<TypeID>> resolveInterfaceGenericInstantiationTypeArguments(
        TypeBindContext const& bindContext,
        Module const& interfaceModule,
        InterfaceDeclRecord const& interfaceDecl,
        Array<UniquePtr<parser::TypeExpression>> const& providedArguments,
        parser::ASTNode const& errorNode);
}
