#pragma once

#include <string>
#include <vector>

namespace rune::test
{
    struct ProcessResult final
    {
        int exitCode{0};
        std::string output;
    };

    ProcessResult runProcess(std::vector<std::string> const& arguments);
    std::string shellSingleQuote(std::string const& value);
}
