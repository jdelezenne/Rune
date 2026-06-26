#pragma once

#include "Rune/Core/StringView.hpp"

#include <windows.h>
namespace rune
{
    namespace Platform
    {
        inline void* findSymbol(StringView name)
        {
            return reinterpret_cast<void*>(GetProcAddress(GetModuleHandle(nullptr), name.data()));
        }
    }
}
