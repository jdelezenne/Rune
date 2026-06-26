#pragma once

#if defined(_WIN32)
#include "Rune/Core/Platform/Windows.hpp"
#elif defined(__EMSCRIPTEN__)
#include "Rune/Core/Platform/Emscripten.hpp"
#elif defined(__APPLE__) || defined(__linux__)
// POSIX dlsym implementation, shared by Darwin and Linux.
#include "Rune/Core/Platform/Darwin.hpp"
#else
#error "Unsupported platform"
#endif
