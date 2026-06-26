#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Four-lane single-precision floating-point vector.
///
/// Components are exposed as `x`, `y`, `z`, and `w`. The layout is fixed and
/// portable across platforms.
typedef struct RuneFloat32x4
{
    alignas(16) float32 x; ///< First lane.
    float32 y; ///< Second lane.
    float32 z; ///< Third lane.
    float32 w; ///< Fourth lane.
} RuneFloat32x4;

/// @name Construction
/// @{

/// Initializes `out` with four explicit lane values.
void rune_float32x4_make4(RuneFloat32x4* out, float32 x, float32 y, float32 z, float32 w);

/// Broadcasts `v` into all lanes of `out`.
void rune_float32x4_splat(RuneFloat32x4* out, float32 v);

/// @}

/// @name Lane Access
/// @{

/// Returns the first lane of `v`.
float32 rune_float32x4_get_x(RuneFloat32x4 const* v);

/// Returns the second lane of `v`.
float32 rune_float32x4_get_y(RuneFloat32x4 const* v);

/// Returns the third lane of `v`.
float32 rune_float32x4_get_z(RuneFloat32x4 const* v);

/// Returns the fourth lane of `v`.
float32 rune_float32x4_get_w(RuneFloat32x4 const* v);

/// @}

/// @name Arithmetic
/// @{

/// Writes `a + b` lane-wise into `out`.
void rune_float32x4_add(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// Writes `a - b` lane-wise into `out`.
void rune_float32x4_sub(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// Writes `a * b` lane-wise into `out`.
void rune_float32x4_mul(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// Writes `a / b` lane-wise into `out`.
void rune_float32x4_div(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// @}

/// @name Unary Operations
/// @{

/// Writes the lane-wise square root of `a` into `out`.
void rune_float32x4_sqrt(RuneFloat32x4* out, RuneFloat32x4 const* a);

/// Writes the lane-wise absolute value of `a` into `out`.
void rune_float32x4_abs(RuneFloat32x4* out, RuneFloat32x4 const* a);

/// Writes the lane-wise negation of `a` into `out`.
void rune_float32x4_neg(RuneFloat32x4* out, RuneFloat32x4 const* a);

/// @}

/// @name Component-Wise Min/Max
/// @{

/// Writes the lane-wise minimum of `a` and `b` into `out`.
void rune_float32x4_min(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// Writes the lane-wise maximum of `a` and `b` into `out`.
void rune_float32x4_max(RuneFloat32x4* out, RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// @}

/// @name Horizontal Operations
/// @{

/// Returns the dot product of `a` and `b`.
float32 rune_float32x4_dot(RuneFloat32x4 const* a, RuneFloat32x4 const* b);

/// Returns the sum of all four lanes of `a`.
float32 rune_float32x4_sum(RuneFloat32x4 const* a);

/// @}

#ifdef __cplusplus
}
#endif
