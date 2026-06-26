#pragma once

#include "Rune/Core/StringView.hpp"
#include "Rune/VM/Artifact/Instruction.hpp"

namespace rune::vm::artifact
{
    StringView opcodeName(Opcode opcode);
    StringView binaryOpName(BinaryOp op);
    StringView unaryOpName(UnaryOp op);
    StringView castPolicyName(CastPolicy policy);
}
