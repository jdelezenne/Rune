#pragma once

#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Platform dynamic library handle.
typedef struct RuneSharedLibrary
{
    void* native;     ///< Platform-native handle, or null when closed.
    bool initialized; ///< True after a successful open.
} RuneSharedLibrary;

/// Opens a shared library by platform path.
/// @param self Library storage to initialize. No-op when null or already initialized.
/// @param path Null-terminated platform path to the library.
/// @return True when the library was opened.
bool rune_shared_library_open(RuneSharedLibrary* self, char const* path);

/// Opens the current process image for symbol lookup.
/// @param self Library storage to initialize. No-op when null or already initialized.
/// @return True when the current process image was opened.
bool rune_shared_library_open_current(RuneSharedLibrary* self);

/// Closes a shared library handle.
/// @param self Library to close. No-op when null or uninitialized.
void rune_shared_library_close(RuneSharedLibrary* self);

/// Looks up a symbol in the library.
/// @param self Library to search.
/// @param name Null-terminated symbol name.
/// @return Symbol address, or null when not found.
void* rune_shared_library_lookup(RuneSharedLibrary* self, char const* name);

/// Returns the embedded platform-native handle.
/// @param self Library to inspect.
/// @return Platform-native handle, or null when uninitialized.
void* rune_shared_library_native_handle(RuneSharedLibrary* self);

#ifdef __cplusplus
}
#endif
