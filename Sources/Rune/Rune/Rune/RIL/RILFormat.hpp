#pragma once

#include "Rune/Core/StringView.hpp"
#include "Rune/RIL/Primitives.hpp"

namespace rune
{
    StringView rilTypeKindName(ril::TypeKind kind);
    StringView rilValueKindName(ril::ValueKind kind);
    StringView rilStatementKindName(ril::StatementKind kind);
    StringView rilTerminatorKindName(ril::TerminatorKind kind);
    StringView rilBinaryOperatorName(ril::BinaryOperator op);
    StringView rilUnaryOperatorName(ril::UnaryOperator op);
    StringView rilCastPolicyName(ril::CastPolicy policy);
    StringView rilRangeKindName(ril::RangeKind kind);
    StringView rilIteratorKindName(ril::IteratorKind kind);
    StringView rilCompilerIntrinsicKindName(CompilerIntrinsicKind kind);
}
