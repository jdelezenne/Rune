#pragma once

#include "Testing/ModulePathsTestSetup.hpp"
#include <Rune/SemanticAnalyzer/SemanticAnalyzer.hpp>

#include <cstdlib>
#include <filesystem>
#include <stdexcept>

namespace rune::testing
{
    extern bool g_verboseMode;
}

namespace rune::test
{
    inline bool isVerboseModeEnabled()
    {
        if (rune::testing::g_verboseMode)
        {
            return true;
        }

        char const* envVar = std::getenv("RUNE_TEST_VERBOSE");
        return envVar != nullptr && envVar[0] != '\0';
    }

    inline SemanticAnalyzer::SemanticAnalyzer createSemanticAnalyzer()
    {
        return SemanticAnalyzer::SemanticAnalyzer(isVerboseModeEnabled());
    }

}
