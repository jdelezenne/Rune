#pragma once

#include "Rune/Parser/AST.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::package
{
    /// Load dependency modules that every Rune compilation implicitly depends on.
    bool preloadImplicitDependencies(ASTContext& context);

    /// Load one explicit dependency interface by artifact or manifest path before import binding.
    bool preloadDependency(
        ASTContext& context,
        String const& dependencyPath,
        Array<String> const& moduleSearchPaths);

    /// Load explicit dependency interfaces by artifact or manifest path before import binding.
    bool preloadDependencies(ASTContext& context, Array<String> const& dependencyPaths);

    /// Make every implicit module visible in every source file.
    bool bindImplicitImports(ASTContext& context, Array<String> const& alwaysImported);

    /// Bind `import` statements to module dependency modules.
    bool bindImports(ASTContext& context, parser::Program const& program);
}
