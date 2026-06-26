#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @name Basic Math
/// @{

/// Absolute value of a floating-point value.
/// @param x Input value.
/// @return Non-negative magnitude.
float64 rune_math_abs(float64 x);

/// Absolute value (Float32).
/// @param x Input value.
/// @return Non-negative magnitude.
float32 rune_math_absf(float32 x);

/// Square root.
/// @param x Non-negative input.
/// @return Square root of x.
float64 rune_math_sqrt(float64 x);

/// Raises base to exponent.
/// @param base Base value.
/// @param exponent Exponent value.
/// @return base raised to exponent.
float64 rune_math_pow(float64 base, float64 exponent);

/// Square root (Float32).
/// @param x Non-negative input.
/// @return Square root of x.
float32 rune_math_sqrtf(float32 x);

/// Raises base to exponent (Float32).
/// @param base Base value.
/// @param exponent Exponent value.
/// @return base raised to exponent.
float32 rune_math_powf(float32 base, float32 exponent);

/// @}

/// @name Trigonometric Functions (Float64)
/// @{
float64 rune_math_sin(float64 x);
float64 rune_math_cos(float64 x);
float64 rune_math_tan(float64 x);
float64 rune_math_asin(float64 x);
float64 rune_math_acos(float64 x);
float64 rune_math_atan(float64 x);
float64 rune_math_atan2(float64 y, float64 x);
/// @}

/// @name Trigonometric Functions (Float32)
/// @{
float32 rune_math_sinf(float32 x);
float32 rune_math_cosf(float32 x);
float32 rune_math_tanf(float32 x);
float32 rune_math_asinf(float32 x);
float32 rune_math_acosf(float32 x);
float32 rune_math_atanf(float32 x);
float32 rune_math_atan2f(float32 y, float32 x);
/// @}

/// @name Hyperbolic Functions (Float64)
/// @{
float64 rune_math_sinh(float64 x);
float64 rune_math_cosh(float64 x);
float64 rune_math_tanh(float64 x);
float64 rune_math_asinh(float64 x);
float64 rune_math_acosh(float64 x);
float64 rune_math_atanh(float64 x);
/// @}

/// @name Hyperbolic Functions (Float32)
/// @{
float32 rune_math_sinhf(float32 x);
float32 rune_math_coshf(float32 x);
float32 rune_math_tanhf(float32 x);
float32 rune_math_asinhf(float32 x);
float32 rune_math_acoshf(float32 x);
float32 rune_math_atanhf(float32 x);
/// @}

/// @name Exponential and Logarithmic Functions (Float64)
/// @{
float64 rune_math_exp(float64 x);
float64 rune_math_exp2(float64 x);
float64 rune_math_log(float64 x);
float64 rune_math_log10(float64 x);
float64 rune_math_log2(float64 x);
float64 rune_math_log1p(float64 x);
/// @}

/// @name Exponential and Logarithmic Functions (Float32)
/// @{
float32 rune_math_expf(float32 x);
float32 rune_math_exp2f(float32 x);
float32 rune_math_logf(float32 x);
float32 rune_math_log10f(float32 x);
float32 rune_math_log2f(float32 x);
float32 rune_math_log1pf(float32 x);
/// @}

/// @name Power and Root Functions (Float64)
/// @{
float64 rune_math_cbrt(float64 x);
float64 rune_math_hypot(float64 x, float64 y);
/// @}

/// @name Power and Root Functions (Float32)
/// @{
float32 rune_math_cbrtf(float32 x);
float32 rune_math_hypotf(float32 x, float32 y);
/// @}

/// @name Rounding and Remainder Functions (Float64)
/// @{
float64 rune_math_round(float64 x);
float64 rune_math_trunc(float64 x);
float64 rune_math_floor(float64 x);
float64 rune_math_ceil(float64 x);
float64 rune_math_fmod(float64 x, float64 y);
float64 rune_math_remainder(float64 x, float64 y);
/// @}

/// @name Rounding and Remainder Functions (Float32)
/// @{
float32 rune_math_roundf(float32 x);
float32 rune_math_truncf(float32 x);
float32 rune_math_floorf(float32 x);
float32 rune_math_ceilf(float32 x);
float32 rune_math_fmodf(float32 x, float32 y);
float32 rune_math_remainderf(float32 x, float32 y);
/// @}

/// @name Absolute and Sign Functions (Float64)
/// @{
float64 rune_math_copysign(float64 x, float64 y);
/// @}

/// @name Absolute and Sign Functions (Float32)
/// @{
float32 rune_math_copysignf(float32 x, float32 y);
/// @}

/// @name Comparison and Classification Functions (Float64)
/// @{
float64 rune_math_fmax(float64 x, float64 y);
float64 rune_math_fmin(float64 x, float64 y);
float64 rune_math_fdim(float64 x, float64 y);
bool rune_math_isfinite(float64 x);
bool rune_math_isinf(float64 x);
bool rune_math_isnan(float64 x);
/// @}

/// @name Comparison and Classification Functions (Float32)
/// @{
float32 rune_math_fmaxf(float32 x, float32 y);
float32 rune_math_fminf(float32 x, float32 y);
float32 rune_math_fdimf(float32 x, float32 y);
bool rune_math_isfinitef(float32 x);
bool rune_math_isinff(float32 x);
bool rune_math_isnanf(float32 x);
/// @}

/// @name Basic Math (Float128)
/// @{

/// Absolute value (`float128`).
float128 rune_math_absl(float128 x);

/// Square root (`float128`).
float128 rune_math_sqrtl(float128 x);

/// Raises base to exponent (`float128`).
float128 rune_math_powl(float128 base, float128 exponent);

/// Maximum of two values (`float128`).
float128 rune_math_fmaxl(float128 x, float128 y);

/// Minimum of two values (`float128`).
float128 rune_math_fminl(float128 x, float128 y);

/// Check if finite (`float128`).
bool rune_math_isfinited(float128 x);

/// Check if infinite (`float128`).
bool rune_math_isinfl(float128 x);

/// Check if NaN (`float128`).
bool rune_math_isnanl(float128 x);

/// Clamps value to the inclusive range [minVal, maxVal] (`float128`).
float128 rune_math_clamp_float128(float128 value, float128 minVal, float128 maxVal);

/// Linear interpolation (`float128`).
float128 rune_math_lerpl(float128 a, float128 b, float128 t);

/// @}

/// @name Utility Functions (Float64)
/// @{

/// Linear interpolation between a and b.
/// @param a Start value.
/// @param b End value.
/// @param t Interpolation factor in [0, 1].
/// @return Interpolated value.
float64 rune_math_lerp(float64 a, float64 b, float64 t);

/// Clamps value to the inclusive range [minVal, maxVal].
/// @param value Value to clamp.
/// @param minVal Lower bound.
/// @param maxVal Upper bound.
/// @return Clamped value.
float64 rune_math_clamp_float64(float64 value, float64 minVal, float64 maxVal);

/// Converts degrees to radians.
/// @param degrees Angle in degrees.
/// @return Angle in radians.
float64 rune_math_radians(float64 degrees);

/// Converts radians to degrees.
/// @param radians Angle in radians.
/// @return Angle in degrees.
float64 rune_math_degrees(float64 radians);

/// Fused multiply-add: x * y + z.
/// @param x First multiplicand.
/// @param y Second multiplicand.
/// @param z Addend.
/// @return x * y + z.
float64 rune_math_fma(float64 x, float64 y, float64 z);

/// @}

/// @name Utility Functions (Float32)
/// @{

/// Linear interpolation between a and b.
/// @param a Start value.
/// @param b End value.
/// @param t Interpolation factor in [0, 1].
/// @return Interpolated value.
float32 rune_math_lerpf(float32 a, float32 b, float32 t);

/// Clamps value to the inclusive range [minVal, maxVal].
/// @param value Value to clamp.
/// @param minVal Lower bound.
/// @param maxVal Upper bound.
/// @return Clamped value.
float32 rune_math_clamp_float32(float32 value, float32 minVal, float32 maxVal);

/// Converts degrees to radians.
/// @param degrees Angle in degrees.
/// @return Angle in radians.
float32 rune_math_radiansf(float32 degrees);

/// Converts radians to degrees.
/// @param radians Angle in radians.
/// @return Angle in degrees.
float32 rune_math_degreesf(float32 radians);

/// Fused multiply-add: x * y + z.
/// @param x First multiplicand.
/// @param y Second multiplicand.
/// @param z Addend.
/// @return x * y + z.
float32 rune_math_fmaf(float32 x, float32 y, float32 z);

/// @}

/// @name Integer Utility
/// @{

/// Returns the larger of two signed integers.
/// @param x First value.
/// @param y Second value.
/// @return max(x, y).
int64 rune_math_max_int64(int64 x, int64 y);

/// Returns the smaller of two signed integers.
/// @param x First value.
/// @param y Second value.
/// @return min(x, y).
int64 rune_math_min_int64(int64 x, int64 y);

/// Clamps value to the inclusive range [minVal, maxVal].
/// @param value Value to clamp.
/// @param minVal Lower bound.
/// @param maxVal Upper bound.
/// @return Clamped value.
int64 rune_math_clamp_int64(int64 value, int64 minVal, int64 maxVal);

/// Returns the absolute value of a signed integer.
/// @param x Input value.
/// @return Non-negative magnitude.
int64 rune_math_abs_int64(int64 x);

/// @}

#ifdef __cplusplus
}
#endif
