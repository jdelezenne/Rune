#pragma once

#include "Rune/Modules/InspectModule.hpp"
#include "Rune/Sema/SemanticDecl.hpp"

namespace rune
{
    StringView semanticTypeKindName(SemanticTypeKind kind);
    StringView semanticReferenceKindName(SemanticReferenceKind kind);
    StringView semanticDeclKindName(sema::SemanticDeclKind kind);
    StringView declContextKindName(DeclContextKind kind);
    String typeReferenceString(Module const& module, TypeID typeId);
}
