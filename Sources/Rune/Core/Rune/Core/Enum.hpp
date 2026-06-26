#pragma once

#include "Rune/Core/Types.hpp"

/// Defines bitwise operators for an enum class to allow using it as a bitmask/flags.
/// @param EnumName The name of the enum class.
/// @param UnderlyingType The underlying integer type of the enum class.
#define RUNE_DEFINE_ENUM_BITWISE_OPERATORS(EnumName, UnderlyingType)                          \
    constexpr EnumName operator|(EnumName left, EnumName right)                               \
    {                                                                                         \
        return static_cast<EnumName>(                                                         \
            static_cast<UnderlyingType>(left) | static_cast<UnderlyingType>(right));          \
    }                                                                                         \
    constexpr EnumName operator&(EnumName left, EnumName right)                               \
    {                                                                                         \
        return static_cast<EnumName>(                                                         \
            static_cast<UnderlyingType>(left) & static_cast<UnderlyingType>(right));          \
    }                                                                                         \
    constexpr EnumName operator^(EnumName left, EnumName right)                               \
    {                                                                                         \
        return static_cast<EnumName>(                                                         \
            static_cast<UnderlyingType>(left) ^ static_cast<UnderlyingType>(right));          \
    }                                                                                         \
    constexpr EnumName operator~(EnumName value)                                              \
    {                                                                                         \
        return static_cast<EnumName>(~static_cast<UnderlyingType>(value));                    \
    }                                                                                         \
    constexpr EnumName& operator|=(EnumName& left, EnumName right)                            \
    {                                                                                         \
        left = left | right;                                                                  \
        return left;                                                                          \
    }                                                                                         \
    constexpr EnumName& operator&=(EnumName& left, EnumName right)                            \
    {                                                                                         \
        left = left & right;                                                                  \
        return left;                                                                          \
    }                                                                                         \
    constexpr EnumName& operator^=(EnumName& left, EnumName right)                            \
    {                                                                                         \
        left = left ^ right;                                                                  \
        return left;                                                                          \
    }                                                                                         \
    constexpr bool hasFlag(EnumName flags, EnumName flag)                                     \
    {                                                                                         \
        return (static_cast<UnderlyingType>(flags) & static_cast<UnderlyingType>(flag)) != 0; \
    }
