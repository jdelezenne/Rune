#pragma once

#include "Rune/Core/StringView.hpp"

#include <dlfcn.h>
namespace rune
{
    namespace Platform
    {
        inline void* findSymbol(StringView name)
        {
            return dlsym(RTLD_DEFAULT, name.data());
        }
    }
}
