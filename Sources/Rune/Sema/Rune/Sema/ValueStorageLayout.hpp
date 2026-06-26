#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/HashSet.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    enum class StructLayoutKind : uint8
    {
        Default,
        C,
        Packed,
    };

    struct StructLayoutPolicy final
    {
        StructLayoutKind kind{StructLayoutKind::Default};
        Optional<uint32> explicitAlignment;
    };

    struct StorageLayout final
    {
        uint32 size{0};
        uint32 alignment{1};
    };

    struct InlineArrayStorageLayout final
    {
        StorageLayout storage;
        StorageLayout element;
        uint32 elementCount{0};
        uint32 elementStride{0};
    };

    struct FieldStorageLayout final
    {
        parser::VariableDeclaration const* decl{nullptr};
        IdentifierID fieldNameId{0};
        TypeID typeId{};
        uint32 offset{0};
        uint32 size{0};
        uint32 naturalAlignment{1};
        uint32 effectiveAlignment{1};
        bool requiresUnalignedAccess{false};
    };

    struct NominalStorageLayout final
    {
        StorageLayout storage;
        StructLayoutPolicy policy;
        Array<FieldStorageLayout> fields;
    };

    StructLayoutPolicy structLayoutPolicyForDecl(parser::StructDeclaration const& structDecl);

    bool typeHasCLayoutPolicy(ASTContext const& context, TypeID typeId);

    /// Computes the inline C storage layout for a type.
    /// Returns empty when the type cannot be sized at compile time.
    Optional<StorageLayout> storageLayoutOfType(
        ASTContext const& context,
        TypeID typeId,
        HashMap<TypeID, StorageLayout>& cache,
        HashSet<TypeID>& active);

    Optional<StorageLayout> storageLayoutOfType(ASTContext const& context, TypeID typeId);

    Optional<InlineArrayStorageLayout> inlineArrayStorageLayoutOfType(
        ASTContext const& context,
        TypeID typeId);

    Optional<StorageLayout> cAbiStorageLayoutOfType(ASTContext const& context, TypeID typeId);

    Optional<NominalStorageLayout> nominalStorageLayoutOfStruct(
        ASTContext const& context,
        TypeID typeId);
}
