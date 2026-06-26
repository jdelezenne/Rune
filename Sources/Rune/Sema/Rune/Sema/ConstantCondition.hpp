#pragma once

#include "Rune/Core/Optional.hpp"

namespace rune
{
    class ASTContext;

    namespace parser
    {
        class Expression;
    }
}

namespace rune::sema
{
    /// Evaluates whether `condition` is provably a compile-time-constant Bool: a
    /// literal `true`/`false`, or a builtin macro call folded to a constant (e.g.
    /// `#platform(...)`). Returns `nullopt` when the condition is not statically known.
    ///
    /// Callers that find a value here must treat the untaken branch as dead: skip both
    /// semantic analysis and code generation for it, so it may reference declarations
    /// that do not exist for the platform actually being compiled.
    Optional<bool> evaluateConstantBoolCondition(ASTContext const& context, parser::Expression const& condition);
}
