#pragma once

#if defined(_WIN32)
#include "Rune/Core/Platform/Windows.hpp"
#elif defined(__APPLE__)
#include "Rune/Core/Platform/Darwin.hpp"
#else
#error "Unsupported platform"
#endif
