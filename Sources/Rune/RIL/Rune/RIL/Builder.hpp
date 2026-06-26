#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Eval.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::ril
{
    struct ModuleBuilder;

    /// Lower `program` into `builder`. On success, `builder.module` is ready
    /// for backend emit. The same builder may afterward lower package
    /// serialized bodies (symbolic generics use a separate cache key).
    Result<bool> buildModuleInto(
        ModuleBuilder& builder,
        parser::Program const& program);

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
