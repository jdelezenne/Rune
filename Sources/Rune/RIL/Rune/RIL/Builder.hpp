#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Eval.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::ril
{
    Result<Module> buildModule(
        ASTContext const& context,
        parser::Program const& program,
        MaterializedEvalValues const* materializedEvalValues = nullptr);

    Result<EvaluationModule> buildEvaluationModule(
        ASTContext const& context,
        parser::Program const& program,
        parser::Expression const& expression,
        String moduleName = "Eval");
}
