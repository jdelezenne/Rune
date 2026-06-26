#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

namespace rune::vm::interpreter::trace
{
    void moduleExecute(artifact::Module const& module);
    void functionEnter(
        LoadedProgram const& program,
        LoadedFunction const& function,
        usize depth,
        Array<Value> const& arguments);
    void instruction(
        LoadedProgram const& program,
        LoadedFunction const& function,
        usize depth,
        uint32 ip,
        LoadedInstruction const& loadedInstruction);
    void functionReturn(
        LoadedProgram const& program,
        LoadedFunction const& function,
        usize depth,
        Optional<Value> const& value);
}
