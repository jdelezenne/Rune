#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

namespace rune::vm::interpreter
{
    Result<bool> tryExecuteRuntimeOperation(
        LoadedAbi const& abi,
        Array<Value> const& arguments,
        Value& result);
}
