#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/VM/Artifact/Module.hpp"
#include "Rune/VM/Interpreter/Frame.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"

namespace rune::vm::interpreter
{
    enum class DebugAction : uint8
    {
        Continue,
        Terminate,
    };

    class DebugSession
    {
    public:
        virtual ~DebugSession() = default;
        virtual Result<DebugAction> beforeInstruction(
            LoadedProgram& program,
            Array<RuntimeFrame>& callStack) = 0;
    };

    struct ExecuteOptions final
    {
        bool trace{false};
        DebugSession* debugSession{nullptr};
    };

    Result<int> execute(LoadedProgram& program, FunctionHandle entry, ExecuteOptions const& options = {});
    Result<int> executeModule(artifact::Module const& module, ExecuteOptions const& options = {});
}
