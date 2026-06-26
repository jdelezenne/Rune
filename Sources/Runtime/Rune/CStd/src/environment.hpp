#pragma once

#include <rune/array.h>
#include <rune/string.h>

#include <cstdlib>
#include <cstring>

extern RuneArray g_arguments;
extern RuneString g_executablePath;

namespace env
{
    inline char const* stringData(RuneString const* string)
    {
        return string != nullptr ? rune_string_data(const_cast<RuneString*>(string)) : "";
    }

    inline void assignBytes(RuneString* out, char const* bytes, usize length)
    {
        if (out == nullptr)
        {
            return;
        }
        rune_string_assign_bytes(out, bytes != nullptr ? bytes : "", static_cast<uint64>(length));
    }

    inline void assignPath(RuneString* out, char const* path)
    {
        if (path == nullptr)
        {
            assignBytes(out, "", 0);
            return;
        }
        assignBytes(out, path, std::strlen(path));
    }

    inline char* duplicateBytes(char const* bytes, usize length)
    {
        if (bytes == nullptr || length == 0)
        {
            char* empty = static_cast<char*>(std::malloc(1));
            if (empty != nullptr)
            {
                empty[0] = '\0';
            }

            return empty;
        }
        char* copy = static_cast<char*>(std::malloc(length + 1));
        if (copy == nullptr)
        {
            return nullptr;
        }
        std::memcpy(copy, bytes, length);
        copy[length] = '\0';
        return copy;
    }

    inline char* duplicateString(RuneString const* string)
    {
        if (string == nullptr)
        {
            return duplicateBytes(nullptr, 0);
        }
        char const* data = rune_string_data(const_cast<RuneString*>(string));
        uint64 const length = rune_string_byte_length(const_cast<RuneString*>(string));
        return duplicateBytes(data, static_cast<usize>(length));
    }

    inline void writeErrorMessage(RuneString* out, char const* message)
    {
        assignPath(out, message != nullptr ? message : "unknown error");
    }

    inline char const* baseName(char const* path)
    {
        if (path == nullptr || path[0] == '\0')
        {
            return "";
        }
        char const* slash = std::strrchr(path, '/');
#if defined(_WIN32)
        char const* backslash = std::strrchr(path, '\\');
        if (backslash != nullptr && (slash == nullptr || backslash > slash))
        {
            slash = backslash;
        }
#endif
        return slash != nullptr ? slash + 1 : path;
    }
}
