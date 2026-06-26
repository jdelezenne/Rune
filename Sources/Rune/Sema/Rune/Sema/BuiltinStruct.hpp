#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune::sema
{
    /// @builtin struct types backed by fixed C storage (e.g. Mutex -> RuneMutex).
    /// These are not KnownStandardTypeKind entries.
    Optional<StringView> opaqueBuiltinRuntimeTypeName(ASTContext const& context, TypeID typeId);

    /// Runtime C storage backing for builtin/standard library value types.
    Optional<StringView> builtinRuntimeStorageTypeName(ASTContext const& context, TypeID typeId);
}
