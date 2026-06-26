#pragma once

#include "Rune/Core/Fnv1a.hpp"

namespace rune
{
    /// Computes a compile-time FNV-1a hash for a type name.
    /// @param str Type name text.
    /// @return 64-bit type hash.
    constexpr uint64 computeTypeHash(StringView str)
    {
        return Fnv1a::staticHash(str);
    }

    /// User-defined literal that hashes a type name at compile time.
    /// @param str Type name text.
    /// @return 64-bit type hash.
    constexpr uint64 operator""_type(char const* str, usize)
    {
        return computeTypeHash(str);
    }

    /// Unique compile-time type identifier derived from a type name hash.
    using TypeId = uint64;

    /// Layout and identity metadata for a Rune type.
    struct TypeInfo final
    {
        TypeId id;        ///< Unique type identifier (hash).
        usize size;       ///< Aligned size in bytes with padding.
        usize packedSize; ///< Sum of field sizes without padding.
        usize alignment;  ///< Memory alignment requirement.
        StringView name;  ///< Type name.

        /// Constructs type information with explicit packed size.
        /// @param id Unique type identifier.
        /// @param size Aligned size in bytes.
        /// @param packedSize Unpadded field size sum.
        /// @param alignment Memory alignment requirement.
        /// @param name Type name.
        constexpr TypeInfo(TypeId id, usize size, usize packedSize, usize alignment, StringView name)
            : id(id)
            , size(size)
            , packedSize(packedSize)
            , alignment(alignment)
            , name(name)
        {
        }

        /// Constructs type information where packed size equals aligned size.
        /// @param id Unique type identifier.
        /// @param size Aligned size in bytes.
        /// @param alignment Memory alignment requirement.
        /// @param name Type name.
        constexpr TypeInfo(TypeId id, usize size, usize alignment, StringView name)
            : TypeInfo(id, size, size, alignment, name)
        {
        }

        /// Returns the number of trailing padding bytes.
        /// @return Padding bytes between packed and aligned size.
        constexpr usize getPaddingBytes() const
        {
            return size - packedSize;
        }
    };
}
