#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Memory ordering constraints for atomic operations.
///
/// Semantics match C11 `<stdatomic.h>` and C++ `std::memory_order`. Passing an
/// order that is invalid for the given operation (e.g. `Release` on a load)
/// results in undefined behaviour.
typedef enum RuneMemoryOrder
{
    RuneMemoryOrderRelaxed = 0, ///< No synchronization; only atomicity is guaranteed.
    RuneMemoryOrderAcquire = 1, ///< Load: no later access may be reordered before this load.
    RuneMemoryOrderRelease = 2, ///< Store: no earlier access may be reordered after this store.
    RuneMemoryOrderAcqRel  = 3, ///< Acquire + Release combined; valid only for read-modify-write ops.
    RuneMemoryOrderSeqCst  = 4, ///< Full sequential consistency across all `SeqCst` operations.
} RuneMemoryOrder;

/// Atomic boolean. Zero-initialize to start in a defined state.
typedef struct RuneAtomicBool
{
    bool value;
} RuneAtomicBool;

/// Atomic 32-bit signed integer. Zero-initialize to start in a defined state.
typedef struct RuneAtomicInt32
{
    int32 value;
} RuneAtomicInt32;

/// Atomic 32-bit unsigned integer. Zero-initialize to start in a defined state.
typedef struct RuneAtomicUInt32
{
    uint32 value;
} RuneAtomicUInt32;

/// Atomic 64-bit signed integer. Zero-initialize to start in a defined state.
typedef struct RuneAtomicInt64
{
    int64 value;
} RuneAtomicInt64;

/// Atomic 64-bit unsigned integer. Zero-initialize to start in a defined state.
typedef struct RuneAtomicUInt64
{
    uint64 value;
} RuneAtomicUInt64;

/// Atomic pointer-sized unsigned integer. Zero-initialize to start in a defined state.
typedef struct RuneAtomicPointer
{
    uintptr value;
} RuneAtomicPointer;

/// @name RuneAtomicBool
/// @{

/// Atomically loads and returns the value.
bool rune_atomic_bool_load(RuneAtomicBool const* self, RuneMemoryOrder order);
/// Atomically stores `value`.
void rune_atomic_bool_store(RuneAtomicBool* self, bool value, RuneMemoryOrder order);
/// Atomically stores `value` and returns the previous value.
bool rune_atomic_bool_exchange(RuneAtomicBool* self, bool value, RuneMemoryOrder order);
/// Atomically stores `desired` if the current value equals `expected`.
/// Returns the value that was in `self` before the operation.
/// If the returned value equals `expected`, the exchange succeeded.
bool rune_atomic_bool_compare_exchange(RuneAtomicBool* self, bool expected, bool desired, RuneMemoryOrder order);

/// @}

/// @name RuneAtomicInt32
/// @{

/// Atomically loads and returns the value.
int32 rune_atomic_int32_load(RuneAtomicInt32 const* self, RuneMemoryOrder order);
/// Atomically stores `value`.
void rune_atomic_int32_store(RuneAtomicInt32* self, int32 value, RuneMemoryOrder order);
/// Atomically stores `value` and returns the previous value.
int32 rune_atomic_int32_exchange(RuneAtomicInt32* self, int32 value, RuneMemoryOrder order);
/// Atomically stores `desired` if the current value equals `expected`.
/// Returns the value that was in `self` before the operation.
/// If the returned value equals `expected`, the exchange succeeded.
int32 rune_atomic_int32_compare_exchange(RuneAtomicInt32* self, int32 expected, int32 desired, RuneMemoryOrder order);
/// Atomically adds `value` and returns the previous value.
int32 rune_atomic_int32_fetch_add(RuneAtomicInt32* self, int32 value, RuneMemoryOrder order);
/// Atomically subtracts `value` and returns the previous value.
int32 rune_atomic_int32_fetch_sub(RuneAtomicInt32* self, int32 value, RuneMemoryOrder order);

/// @}

/// @name RuneAtomicUInt32
/// @{

uint32 rune_atomic_uint32_load(RuneAtomicUInt32 const* self, RuneMemoryOrder order);
void rune_atomic_uint32_store(RuneAtomicUInt32* self, uint32 value, RuneMemoryOrder order);
uint32 rune_atomic_uint32_exchange(RuneAtomicUInt32* self, uint32 value, RuneMemoryOrder order);
uint32 rune_atomic_uint32_compare_exchange(RuneAtomicUInt32* self, uint32 expected, uint32 desired, RuneMemoryOrder order);
uint32 rune_atomic_uint32_fetch_add(RuneAtomicUInt32* self, uint32 value, RuneMemoryOrder order);
uint32 rune_atomic_uint32_fetch_sub(RuneAtomicUInt32* self, uint32 value, RuneMemoryOrder order);

/// @}

/// @name RuneAtomicInt64
/// @{

int64 rune_atomic_int64_load(RuneAtomicInt64 const* self, RuneMemoryOrder order);
void rune_atomic_int64_store(RuneAtomicInt64* self, int64 value, RuneMemoryOrder order);
int64 rune_atomic_int64_exchange(RuneAtomicInt64* self, int64 value, RuneMemoryOrder order);
int64 rune_atomic_int64_compare_exchange(RuneAtomicInt64* self, int64 expected, int64 desired, RuneMemoryOrder order);
int64 rune_atomic_int64_fetch_add(RuneAtomicInt64* self, int64 value, RuneMemoryOrder order);
int64 rune_atomic_int64_fetch_sub(RuneAtomicInt64* self, int64 value, RuneMemoryOrder order);

/// @}

/// @name RuneAtomicUInt64
/// @{

uint64 rune_atomic_uint64_load(RuneAtomicUInt64 const* self, RuneMemoryOrder order);
void rune_atomic_uint64_store(RuneAtomicUInt64* self, uint64 value, RuneMemoryOrder order);
uint64 rune_atomic_uint64_exchange(RuneAtomicUInt64* self, uint64 value, RuneMemoryOrder order);
uint64 rune_atomic_uint64_compare_exchange(RuneAtomicUInt64* self, uint64 expected, uint64 desired, RuneMemoryOrder order);
uint64 rune_atomic_uint64_fetch_add(RuneAtomicUInt64* self, uint64 value, RuneMemoryOrder order);
uint64 rune_atomic_uint64_fetch_sub(RuneAtomicUInt64* self, uint64 value, RuneMemoryOrder order);

/// @}

/// @name RuneAtomicPointer
/// @{

/// Atomically loads and returns the value.
uintptr rune_atomic_pointer_load(RuneAtomicPointer const* self, RuneMemoryOrder order);
/// Atomically stores `value`.
void rune_atomic_pointer_store(RuneAtomicPointer* self, uintptr value, RuneMemoryOrder order);
/// Atomically stores `value` and returns the previous value.
uintptr rune_atomic_pointer_exchange(RuneAtomicPointer* self, uintptr value, RuneMemoryOrder order);
/// Atomically stores `desired` if the current value equals `expected`.
/// Returns the value that was in `self` before the operation.
/// If the returned value equals `expected`, the exchange succeeded.
uintptr rune_atomic_pointer_compare_exchange(RuneAtomicPointer* self, uintptr expected, uintptr desired, RuneMemoryOrder order);

/// @}

/// Issues a memory fence with the specified ordering.
void rune_atomic_fence(RuneMemoryOrder order);

#ifdef __cplusplus
}
#endif
