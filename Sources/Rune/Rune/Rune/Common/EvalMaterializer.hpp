#pragma once

#include "Rune/RIL/Eval.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    Result<ril::MaterializedEvalValues> materializeEvalExpressions(
        ASTContext const& context,
        parser::Program const& program);
}
