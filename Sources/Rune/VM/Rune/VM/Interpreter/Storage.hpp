#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/VM/Artifact/Module.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

namespace rune::vm::interpreter
{
    Result<bool> writeValueToStorage(
        uint8* storage,
        uint32 byteSize,
        Value const& value);

    Result<Value> readValueFromStorage(
        artifact::TypeRecord const& type,
        uint8 const* storage,
        uint32 byteSize);

    Result<bool> writeValueBytes(
        Array<uint8>& storage,
        uint32 byteOffset,
        artifact::TypeRecord const& type,
        Value const& value);

    Result<Value> readValueBytes(
        Array<uint8> const& storage,
        uint32 byteOffset,
        artifact::TypeRecord const& type);
}
