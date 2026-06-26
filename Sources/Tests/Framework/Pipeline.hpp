#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Lexer/Lexer.hpp"
#include "Rune/Parser/AST.hpp"

namespace rune::test
{
    Result<Array<lexer::Token>> tokenize(String const& source);

    Result<UniquePtr<parser::Program>> parse(String const& source);

    /// Parses source and returns the program even when recoverable errors were recorded.
    Result<UniquePtr<parser::Program>> parseWithRecovery(String const& source);
}
