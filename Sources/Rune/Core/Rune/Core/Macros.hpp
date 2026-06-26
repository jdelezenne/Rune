#pragma once

/// Generates a unique identifier by concatenating str with the current source line.
#define RUNE_ANONYMOUS_VARIABLE(str) RUNE_CONCAT(str, __LINE__)

#define RUNE_CONCAT_IMPL(s1, s2) s1##s2
/// Concatenates two preprocessor tokens.
#define RUNE_CONCAT(s1, s2) RUNE_CONCAT_IMPL(s1, s2)

/// Declares default copy constructor and copy assignment for Type.
#define RUNE_COPYABLE(Type)      \
    Type(Type const&) = default; \
    Type& operator=(Type const&) = default;

/// Declares default move constructor and move assignment for Type.
#define RUNE_MOVABLE(Type)           \
    Type(Type&&) noexcept = default; \
    Type& operator=(Type&&) noexcept = default;

/// Deletes copy constructor and copy assignment for Type.
#define RUNE_NOT_COPYABLE(Type) \
    Type(Type const&) = delete; \
    Type& operator=(Type const&) = delete;

/// Move-only type: default move operations and deleted copy operations.
#define RUNE_MOVE_ONLY(Type) \
    RUNE_MOVABLE(Type)       \
    RUNE_NOT_COPYABLE(Type)

/// Deletes move constructor and move assignment for Type.
#define RUNE_NOT_MOVABLE(Type) \
    Type(Type&&) = delete;     \
    Type& operator=(Type&&) = delete;

/// Prevents direct construction of interface-like Type.
#define RUNE_INTERFACE(Type) \
    Type() = delete;
