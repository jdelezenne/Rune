#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/VM/Artifact/Instruction.hpp"

namespace rune::vm::interpreter
{
    enum class ValueKind : uint8
    {
        Void,
        I64,
        U64,
        F64,
        Bool,
        Pointer,
        Address,
        Aggregate,
    };

    struct Value final
    {
        ValueKind kind{ValueKind::Void};
        uint64 bits{0};
        Array<uint8> bytes;
    };

    struct Address final
    {
        uint32 storageIndex{artifact::invalidIndex};
        uint32 byteOffset{0};
    };

    Result<uint64> pointerBitsFromValue(Value const& value, uint32 pointerByteSize);
}
