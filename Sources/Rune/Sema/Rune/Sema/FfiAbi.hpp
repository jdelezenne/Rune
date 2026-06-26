#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/FfiBindings.hpp"

namespace rune::sema
{
    bool isFfiCCharPointerType(ASTContext const& context, TypeID typeId);

    bool isFfiCStringReferenceType(
        ASTContext const& context,
        TypeID typeId,
        SemanticReferenceKind referenceKind);

    bool isFfiHandleOptionType(ASTContext const& context, TypeID optionTypeId);

    bool isFfiNullablePointerOptionType(ASTContext const& context, TypeID optionTypeId);

    bool isFfiHandleOutSlotParameterType(
        ASTContext const& context,
        TypeID paramTypeId,
        Optional<TypeID>* outOptionTypeId = nullptr);

    bool isFfiMutableLValueParameterType(
        ASTContext const& context,
        TypeID paramTypeId,
        Optional<TypeID>* outPointeeTypeId = nullptr);

    bool ffiHandleOptionTypesMatch(
        ASTContext const& context,
        TypeID leftOptionTypeId,
        TypeID rightOptionTypeId);

    FfiParameterAbi classifyFfiParameterType(ASTContext const& context, TypeID paramTypeId);

    /// Rejected CString reference forms in user C ABI parameters.
    struct FfiCStringBorrowExpectation final
    {
        bool immutable{true};
        bool nullable{false};
    };

    Optional<FfiCStringBorrowExpectation> ffiCStringBorrowExpectation(
        ASTContext const& context,
        TypeID paramTypeId);

    bool isValidFfiCStringBorrowArgument(
        ASTContext const& context,
        parser::Expression const& argumentExpression,
        TypeID argType,
        FfiCStringBorrowExpectation const& expectation);

    Optional<FfiArgumentBinding> bindFfiCallArgument(
        ASTContext const& context,
        parser::Expression const& argumentExpression,
        FfiParameterAbi const& paramAbi,
        Optional<TypeID> knownLocalTypeId = std::nullopt,
        BoundDecl const* knownLocalDecl = nullptr);

    bool ffiCallArgumentRequiresBinding(FfiParameterAbi const& paramAbi);
}
