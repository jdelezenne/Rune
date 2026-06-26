#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - Type declaration

/// Declares a concrete optional type for values of type T, analogous to std::optional<T>.
///
/// Produces a typedef'd struct with two public fields:
///   - `bool has_value` — true when a value is present.
///   - `T value`        — the stored value; valid only when has_value is true.
///
/// The resulting type is a plain value type: it can be returned from functions,
/// stored in structs, and passed by value without heap allocation or type descriptors.
///
/// Example:
///
///     RUNE_OPTION_DECLARE(int32, RuneOptionInt32);
///
///     RuneOptionInt32 result = rune_option_some(RuneOptionInt32, 42);
///     RuneOptionInt32 empty  = rune_option_none(RuneOptionInt32);
///
///     if (rune_option_has_value(result)) {
///         int32 n = rune_option_value(result);
///     }
///     int32 n = rune_option_value_or(result, -1);
#define RUNE_OPTION_DECLARE(T, Name) \
    typedef struct Name              \
    {                                \
        bool has_value;              \
        T value;                     \
    } Name

// MARK: - Construction

#ifdef __cplusplus
/// Constructs a present optional of type Name containing val.
#define rune_option_some(Name, val) (Name{.has_value = true, .value = (val)})
/// Constructs an empty optional of type Name.
#define rune_option_none(Name) (Name{.has_value = false})
/// Assigns a present value to var without naming its type (C++23: decltype).
#define rune_option_assign_some(var, val) ((var) = decltype(var){.has_value = true, .value = (val)})
/// Clears var to empty without naming its type (C++23: decltype).
#define rune_option_assign_none(var) ((var) = decltype(var){.has_value = false})
#else
/// Constructs a present optional of type Name containing val.
#define rune_option_some(Name, val) ((Name){.has_value = true, .value = (val)})
/// Constructs an empty optional of type Name.
#define rune_option_none(Name) ((Name){.has_value = false})
/// Assigns a present value to var without naming its type (C23: typeof).
#define rune_option_assign_some(var, val) ((var) = (typeof(var)){.has_value = true, .value = (val)})
/// Clears var to empty without naming its type (C23: typeof).
#define rune_option_assign_none(var) ((var) = (typeof(var)){.has_value = false})
#endif

// MARK: - Access

/// True when opt contains a value.
#define rune_option_has_value(opt) ((opt).has_value)

/// The stored value. Behaviour is undefined when has_value is false.
#define rune_option_value(opt) ((opt).value)

/// The stored value when present, or fallback when empty.
///
/// Note: both arguments are evaluated. Assign opt to a local variable first
/// if it is a function call, to avoid double evaluation.
#define rune_option_value_or(opt, fallback) ((opt).has_value ? (opt).value : (fallback))

// MARK: - Transform

/// Returns rune_option_some(Name, f(value)) when opt is present, rune_option_none(Name) when empty.
///
/// f must accept the stored value type and return the element type of Name.
#define rune_option_map(opt, Name, f) \
    ((opt).has_value ? rune_option_some(Name, f((opt).value)) : rune_option_none(Name))

// MARK: - Predeclared types

RUNE_OPTION_DECLARE(void*, RuneOptionPtr);
RUNE_OPTION_DECLARE(bool, RuneOptionBool);
RUNE_OPTION_DECLARE(int8, RuneOptionInt8);
RUNE_OPTION_DECLARE(int16, RuneOptionInt16);
RUNE_OPTION_DECLARE(int32, RuneOptionInt32);
RUNE_OPTION_DECLARE(int64, RuneOptionInt64);
RUNE_OPTION_DECLARE(uint8, RuneOptionUInt8);
RUNE_OPTION_DECLARE(uint16, RuneOptionUInt16);
RUNE_OPTION_DECLARE(uint32, RuneOptionUInt32);
RUNE_OPTION_DECLARE(uint64, RuneOptionUInt64);
RUNE_OPTION_DECLARE(float32, RuneOptionFloat32);
RUNE_OPTION_DECLARE(float64, RuneOptionFloat64);
RUNE_OPTION_DECLARE(intptr, RuneOptionIntPtr);
RUNE_OPTION_DECLARE(usize, RuneOptionUSize);

#ifdef __cplusplus
}
#endif
