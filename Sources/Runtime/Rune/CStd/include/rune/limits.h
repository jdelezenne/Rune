#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @name Signed Integer Limits
/// @{
int8 rune_limits_int8_min(void);   ///< Minimum representable `int8` value.
int8 rune_limits_int8_max(void);   ///< Maximum representable `int8` value.
int16 rune_limits_int16_min(void); ///< Minimum representable `int16` value.
int16 rune_limits_int16_max(void); ///< Maximum representable `int16` value.
int32 rune_limits_int32_min(void); ///< Minimum representable `int32` value.
int32 rune_limits_int32_max(void); ///< Maximum representable `int32` value.
int64 rune_limits_int64_min(void);   ///< Minimum representable `int64` value.
int64 rune_limits_int64_max(void);   ///< Maximum representable `int64` value.
intptr rune_limits_int_min(void);    ///< Minimum representable Rune `Int` value.
intptr rune_limits_int_max(void);    ///< Maximum representable Rune `Int` value.
/// @}

/// @name Unsigned Integer Limits
/// @{
uint8 rune_limits_uint8_min(void);   ///< Minimum representable `uint8` value.
uint8 rune_limits_uint8_max(void);   ///< Maximum representable `uint8` value.
uint16 rune_limits_uint16_min(void); ///< Minimum representable `uint16` value.
uint16 rune_limits_uint16_max(void); ///< Maximum representable `uint16` value.
uint32 rune_limits_uint32_min(void); ///< Minimum representable `uint32` value.
uint32 rune_limits_uint32_max(void); ///< Maximum representable `uint32` value.
uint64 rune_limits_uint64_min(void);   ///< Minimum representable `uint64` value.
uint64 rune_limits_uint64_max(void);   ///< Maximum representable `uint64` value.
uintptr rune_limits_uint_min(void);    ///< Minimum representable Rune `UInt` value.
uintptr rune_limits_uint_max(void);    ///< Maximum representable Rune `UInt` value.
/// @}

/// @name Floating-Point Limits
/// @{
float32 rune_limits_float32_min(void);     ///< Minimum normalized positive `float32` value.
float32 rune_limits_float32_max(void);     ///< Maximum finite `float32` value.
float32 rune_limits_float32_lowest(void);  ///< Most negative finite `float32` value.
float32 rune_limits_float32_epsilon(void); ///< Difference between 1.0 and the next representable `float32` value.
float32 rune_limits_float32_inf(void);     ///< Positive infinity for `float32`.
float32 rune_limits_float32_nan(void);     ///< Quiet not-a-number value for `float32`.

float64 rune_limits_float64_min(void);     ///< Minimum normalized positive `float64` value.
float64 rune_limits_float64_max(void);     ///< Maximum finite `float64` value.
float64 rune_limits_float64_lowest(void);  ///< Most negative finite `float64` value.
float64 rune_limits_float64_epsilon(void); ///< Difference between 1.0 and the next representable `float64` value.
float64 rune_limits_float64_inf(void);     ///< Positive infinity for `float64`.
float64 rune_limits_float64_nan(void);     ///< Quiet not-a-number value for `float64`.

float128 rune_limits_float128_min(void);     ///< Minimum normalized positive `float128` value.
float128 rune_limits_float128_max(void);     ///< Maximum finite `float128` value.
float128 rune_limits_float128_lowest(void);  ///< Most negative finite `float128` value.
float128 rune_limits_float128_epsilon(void); ///< Difference between 1.0 and the next representable `float128` value.
float128 rune_limits_float128_inf(void);     ///< Positive infinity for `float128`.
float128 rune_limits_float128_nan(void);     ///< Quiet not-a-number value for `float128`.
/// @}

#ifdef __cplusplus
}
#endif
