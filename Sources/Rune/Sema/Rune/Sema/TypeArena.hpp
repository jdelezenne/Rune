#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Sema/IdentifierTable.hpp"

namespace rune::sema
{
    enum class LiteralCategory : uint8;
}

namespace rune
{
    using TypeID = uint32;

    enum class SemanticTypeKind : uint8
    {
        Error,
        Never,
        Named,
        Generic,
        Function,
        Tuple,
        Reference,
        VariadicPack,
        Literal,
    };

    enum class SemanticReferenceKind : uint8
    {
        Immutable,
        Mutable,
    };

    /// Canonical semantic type (populated during Sema).
    struct SemanticType final
    {
        TypeID id{0};
        SemanticTypeKind kind{SemanticTypeKind::Error};
        IdentifierID nameId{0};
        uint32 nominalDeclId{0};
        IdentifierID cBuiltinStorageNameId{0};
        Array<TypeID> typeArguments;
        Array<IdentifierID> tupleElementNameIds;
        Array<TypeID> parameterTypes;
        TypeID returnTypeId{0};
        bool isAsync{false};
        SemanticReferenceKind referenceKind{SemanticReferenceKind::Immutable};
        sema::LiteralCategory literalCategory{};
    };

    class TypeArena final
    {
    public:
        SemanticType const& errorType();
        SemanticType const& neverType();
        SemanticType const& get(TypeID id) const;
        TypeID internNamed(IdentifierID nameId, uint32 nominalDeclId = 0);
        TypeID internGeneric(
            IdentifierID nameId,
            Array<TypeID> typeArguments,
            uint32 nominalDeclId = 0);
        TypeID internFunction(Array<TypeID> parameterTypes, TypeID returnTypeId, bool isAsync = false);
        TypeID internTuple(Array<TypeID> elementTypes, Array<IdentifierID> elementNameIds = {});
        TypeID internReference(TypeID pointeeId, SemanticReferenceKind referenceKind);
        TypeID internVariadicPack(TypeID elementTypeId);
        TypeID internLiteral(sema::LiteralCategory category);
        void setCBuiltinStorageName(TypeID id, IdentifierID cBuiltinStorageNameId);

        /// Returns an existing canonical type with the same shape, or 0.
        TypeID findMatching(SemanticType const& key) const;

        usize size() const
        {
            return types.size();
        }

    private:
        TypeID allocate(SemanticType type);

        Array<SemanticType> types;
        TypeID nextTypeId{1};
    };
}
