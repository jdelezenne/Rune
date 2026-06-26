#pragma once

#include <rune/array.h>
#include <rune/string.h>

#include <stdlib.h>
#include <string.h>

extern RuneArray g_arguments;
extern RuneString g_executablePath;

static inline char const* env_string_data(RuneString const* string)
{
    return string != nullptr ? rune_string_data((RuneString*)string) : "";
}

static inline void env_assign_bytes(RuneString* out, char const* bytes, usize length)
{
    if (out == nullptr)
    {
        return;
    }

    rune_string_assign_bytes(out, bytes != nullptr ? bytes : "", (uint64)(length));
}

static inline void env_assign_path(RuneString* out, char const* path)
{
    if (path == nullptr)
    {
        env_assign_bytes(out, "", 0);
        return;
    }

    env_assign_bytes(out, path, strlen(path));
}

static inline char* env_duplicate_bytes(char const* bytes, usize length)
{
    if (bytes == nullptr || length == 0)
    {
        char* empty = (char*)(malloc(1));
        if (empty != nullptr)
        {
            empty[0] = '\0';
        }
        return empty;
    }
    char* copy = (char*)(malloc(length + 1));
    if (copy == nullptr)
    {
        return nullptr;
    }
    memcpy(copy, bytes, length);
    copy[length] = '\0';
    return copy;
}

static inline char* env_duplicate_string(RuneString const* string)
{
    if (string == nullptr)
    {
        return env_duplicate_bytes(nullptr, 0);
    }
    char const* data = rune_string_data((RuneString*)string);
    uint64 const length = rune_string_byte_length((RuneString*)string);
    return env_duplicate_bytes(data, (usize)(length));
}

static inline void env_write_error_message(RuneString* out, char const* message)
{
    env_assign_path(out, message != nullptr ? message : "unknown error");
}

static inline char const* env_base_name(char const* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return "";
    }
    char const* slash = strrchr(path, '/');
#if defined(_WIN32)
    char const* backslash = strrchr(path, '\\');
    if (backslash != nullptr && (slash == nullptr || backslash > slash))
    {
        slash = backslash;
    }
#endif
    return slash != nullptr ? slash + 1 : path;
}
