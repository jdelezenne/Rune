#pragma once

#include <rune/string.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

// MARK: - Interface

/// Opaque per-instance state for a `RuneRandom` implementation.
///
/// Each algorithm defines its own concrete struct and casts to/from
/// `RuneRandomContext*` inside its callbacks. Callers never dereference
/// this pointer directly.
typedef struct RuneRandomContext RuneRandomContext;

/// Interface for a pseudo-random number generator.
///
/// To implement a custom generator, fill a `RuneRandomInterface`, provide
/// a concrete context, and store both in a `RuneRandom`.
typedef struct RuneRandomInterface
{
    /// Returns the next 64 uniformly distributed random bits.
    uint64 (*next)(RuneRandomContext* context);

    /// Seeds the generator from a single `uint64`.
    void (*seed)(RuneRandomContext* context, uint64 seed);
} RuneRandomInterface;

/// A pseudo-random number generator.
///
/// `interface` and `context` are set by a concrete open function
/// (e.g. `rune_pcg32_open`). Callers operate through `rune_random_*` functions.
///
/// The context is non-owning; the caller manages the lifetime of the
/// underlying algorithm state (e.g. `RunePcg32State`).
typedef struct RuneRandom
{
    RuneRandomInterface const* interface; ///< Operation dispatch interface.
    RuneRandomContext*          context;   ///< Algorithm state owned by the caller.
} RuneRandom;

// MARK: - PCG32

/// State for the PCG32 pseudo-random number generator.
///
/// PCG32 is a small, fast, high-quality generator with a 64-bit state and
/// a selectable stream. Suitable for simulation, games, and procedural
/// generation.
typedef struct RunePcg32State
{
    uint64 state;    ///< Current generator state.
    uint64 sequence; ///< Stream selector (always odd internally).
} RunePcg32State;

/// Seeds the generator with `seed` on the default stream.
void rune_pcg32_seed(RunePcg32State* self, uint64 seed);

/// Seeds the generator with `seed` on an explicit `sequence`.
/// Different sequences produce independent, non-overlapping output streams.
void rune_pcg32_seed_with_sequence(RunePcg32State* self, uint64 seed, uint64 sequence);

/// Opens a `RuneRandom` interface over `state`.
/// `state` must outlive `random`.
void rune_pcg32_open(RuneRandom* random, RunePcg32State* state);

// MARK: - Xoshiro256**

/// State for the Xoshiro256** pseudo-random number generator.
///
/// Xoshiro256** has a 256-bit state and produces 64-bit output. It is faster
/// than PCG32 for bulk generation and passes all known statistical tests.
/// Suitable when large quantities of random data are needed.
typedef struct RuneXoshiro256State
{
    uint64 s[4]; ///< 256-bit generator state.
} RuneXoshiro256State;

/// Seeds the generator from `seed`, expanded through SplitMix64.
void rune_xoshiro256_seed(RuneXoshiro256State* self, uint64 seed);

/// Opens a `RuneRandom` interface over `state`.
/// `state` must outlive `random`.
void rune_xoshiro256_open(RuneRandom* random, RuneXoshiro256State* state);

// MARK: - RuneRandom operations

/// Seeds the generator through the abstract interface.
void rune_random_seed(RuneRandom* self, uint64 seed);

/// Returns a uniformly distributed random `uint32`.
uint32 rune_random_uint32(RuneRandom* self);

/// Returns a uniformly distributed random `uint64`.
uint64 rune_random_uint64(RuneRandom* self);

/// Returns a uniformly distributed value in `[0, bound)`.
/// Undefined behaviour when `bound` is 0.
uint32 rune_random_uint32_bounded(RuneRandom* self, uint32 bound);

/// Returns a uniformly distributed value in `[0, bound)`.
/// Undefined behaviour when `bound` is 0.
uint64 rune_random_uint64_bounded(RuneRandom* self, uint64 bound);

/// Returns a uniformly distributed `float32` in `[0.0, 1.0)`.
float32 rune_random_float32(RuneRandom* self);

/// Returns a uniformly distributed `float64` in `[0.0, 1.0)`.
float64 rune_random_float64(RuneRandom* self);

/// Fills `buffer` with `size` pseudo-random bytes.
void rune_random_bytes(RuneRandom* self, void* buffer, usize size);

// MARK: - Secure random

/// Fills `buffer` with `size` cryptographically secure random bytes from the
/// OS entropy source (`arc4random_buf`, `getrandom`, or `BCryptGenRandom`).
/// @return True on success.
bool rune_secure_random_bytes(void* buffer, usize size, RuneString* out_error);

#ifdef __cplusplus
}
#endif
