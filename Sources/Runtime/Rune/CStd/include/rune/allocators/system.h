#pragma once

#include <rune/allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Returns the built-in system allocator backed by malloc and free.
///
/// The allocator is stateless: context is always null. It is the default
/// allocator installed on every thread when no override is active.
/// @return System allocator interface.
RuneAllocator rune_system_allocator(void);

#ifdef __cplusplus
}
#endif
