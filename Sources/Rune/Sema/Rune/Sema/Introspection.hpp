#pragma once

#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    /// Classification of a type as surfaced by the introspection macros and the
    /// `.runemodule` introspection section.
    ///
    /// Mirrors the Rune-level `Introspection.TypeKind` enum
    /// (`Code/Modules/Introspection/Introspection.rune`); raw values are part of the
    /// introspection section format - do not renumber.
    enum class IntrospectionTypeKind : uint8
    {
        Unknown = 0,
        Void = 1,
        Bool = 2,
        Integer = 3,
        UnsignedInteger = 4,
        Float = 5,
        Char = 6,
        Byte = 7,
        String = 8,
        Array = 9,
        Slice = 10,
        InlineArray = 11,
        Buffer = 12,
        Map = 13,
        Set = 14,
        Optional = 15,
        Result = 16,
        Range = 17,
        Tuple = 18,
        Function = 19,
        Reference = 20,
        Struct = 21,
        Enum = 22,
        Flags = 23,
        Union = 24,
        Interface = 25,
        Distinct = 26,
        Alias = 27,
        Any = 28,
    };

    /// Rune-level `Introspection.TypeKind` case name for a kind (e.g. `structure`).
    StringView introspectionTypeKindCaseName(IntrospectionTypeKind kind);

    /// Canonical module-qualified spelling of a type, including generic arguments
    /// (e.g. `Rune.Array<Main.Point>`). This spelling is the input of the stable
    /// type identity hash, so it must not depend on how the source spelled the type.
    String introspectionQualifiedTypeName(ASTContext const& context, TypeID typeId);

    /// Stable identity for a canonical qualified type name: the full 64-bit
    /// FNV-1a hash of the spelling (the same hash `String.hash()` uses at
    /// runtime). Never returns 0 so callers can use 0 as "no identity".
    uint64 introspectionTypeIdForQualifiedName(StringView qualifiedName);

    /// Stable identity of a type: `introspectionTypeIdForQualifiedName` of its
    /// canonical qualified name. This is the value produced by `#typeIdOf<T>()`.
    uint64 introspectionTypeId(ASTContext const& context, TypeID typeId);

    /// Classification of a semantic type.
    IntrospectionTypeKind introspectionTypeKindOf(ASTContext const& context, TypeID typeId);

    /// Classification of an exported declaration (used when encoding module records).
    IntrospectionTypeKind introspectionTypeKindForDeclKind(SemanticDeclKind kind, bool isFlagsEnum);

    /// True when the type (recursively) references a generic type parameter and so
    /// has no concrete identity at this use site.
    bool introspectionTypeDependsOnGenericParameter(ASTContext const& context, TypeID typeId);

    /// One case of an enum, flags, or union type as surfaced by introspection.
    struct IntrospectionCaseInfo final
    {
        String name;
        /// Raw value for enum/flags cases; declaration index for union cases.
        int64 value{0};
    };

    /// Raw type of an enum or flags nominal, or the base type of a distinct type.
    /// Empty for other kinds.
    Optional<TypeID> introspectionRawTypeId(ASTContext const& context, TypeID typeId);

    /// Cases of an enum, flags, or union type, whether declared in the current
    /// module or imported through a `.runemodule` interface. Union cases report
    /// their declaration index as the value. Empty for other kinds (and for
    /// imported unions, whose cases are not part of the interface constants).
    Array<IntrospectionCaseInfo> introspectionTypeCases(ASTContext const& context, TypeID typeId);

    /// Introspection record for a concrete semantic type of the current
    /// compilation (`declId` is 0; interface records fill it when encoding a
    /// `.runemodule` section).
    IntrospectionTypeRecord introspectionTypeRecordForType(ASTContext const& context, TypeID typeId);

    /// Every introspectable type of the program being compiled: the current
    /// module's non-generic type declarations plus the records decoded from
    /// imported modules' `.runemodule` introspection sections, de-duplicated
    /// by identity (current module wins).
    Array<IntrospectionTypeRecord> collectProgramIntrospectionTypes(ASTContext const& context);

    /// Serializes introspection records into the introspection section format
    /// (`RINT` magic, version 1, little-endian). The same encoding is used by
    /// the `.runemodule` introspection section, the program artifact's
    /// introspection section, and the runtime decoder in the Introspection
    /// module (`TypeRegistry.init(data:)`).
    Array<uint8> encodeIntrospectionRecords(Array<IntrospectionTypeRecord> const& records);

    /// Decodes an introspection section produced by `encodeIntrospectionRecords`.
    /// Returns false when the bytes are not a valid section.
    bool decodeIntrospectionRecords(
        uint8 const* contents,
        usize size,
        Array<IntrospectionTypeRecord>& records);

    inline bool decodeIntrospectionRecords(
        Array<uint8> const& contents,
        Array<IntrospectionTypeRecord>& records)
    {
        return decodeIntrospectionRecords(contents.data(), contents.size(), records);
    }
}
