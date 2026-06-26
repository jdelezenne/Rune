#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/VM/Artifact/Instruction.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"

#include <array>

namespace rune::vm::interpreter
{
    inline constexpr usize opcodeProfileCount =
        static_cast<usize>(artifact::Opcode::CompilerIntrinsic) + 1u;

    struct ExecutionProfile final
    {
        uint64 instructionCount{0};
        uint64 foreignCallCount{0};
        double foreignCallMs{0.0};
        std::array<uint64, opcodeProfileCount> opcodeCounts{};
        Array<uint64> functionInstructionCounts;
        Array<uint64> foreignCallCounts;
        Array<double> foreignCallTimesMs;
    };

    void printExecutionProfile(
        LoadedProgram const& program,
        ExecutionProfile const& profile,
        double executeMs);
}
