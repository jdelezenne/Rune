#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/// Expands to an opaque struct forward declaration and typedef.
#define RUNE_OPAQUE_TYPE(Name) \
    struct Name;               \
    typedef struct Name Name

/// Opaque forward declarations for parameterized and builtin types referenced
/// through pointers in the public API.
RUNE_OPAQUE_TYPE(RuneArray);
RUNE_OPAQUE_TYPE(RuneHashMap);
RUNE_OPAQUE_TYPE(RuneHashSet);
RUNE_OPAQUE_TYPE(RuneInlineArray);
RUNE_OPAQUE_TYPE(RuneResult);
RUNE_OPAQUE_TYPE(RuneSortedMap);
RUNE_OPAQUE_TYPE(RuneCString);
typedef char const* RuneCStringSlice;
RUNE_OPAQUE_TYPE(RuneSortedSet);
RUNE_OPAQUE_TYPE(RuneString);
RUNE_OPAQUE_TYPE(RuneStringSlice);

#undef RUNE_OPAQUE_TYPE

#ifdef __cplusplus
}
#endif
