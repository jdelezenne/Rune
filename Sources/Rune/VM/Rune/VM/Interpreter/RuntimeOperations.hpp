#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

namespace rune::vm::interpreter
{
    /// Answers runtime operations the interpreter serves itself from the
    /// loaded program (no C symbol dispatch). Returns true when the ABI's
    /// operation was handled and `result` is filled.
    Result<bool> tryExecuteRuntimeOperation(
        LoadedProgram const& program,
        LoadedAbi const& abi,
        Array<Value> const& arguments,
        Value& result);
}
