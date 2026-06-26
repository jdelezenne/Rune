#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/VM/Artifact/Module.hpp"
#include "Rune/VM/Interpreter/Frame.hpp"
#include "Rune/VM/Interpreter/LoadedProgram.hpp"
#include "Rune/VM/Interpreter/Value.hpp"

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
        bool profile{false};
        DebugSession* debugSession{nullptr};
    };

    Result<Value> executeFunction(
        LoadedProgram& program,
        FunctionHandle function,
        Array<Value> const& arguments = {},
        ExecuteOptions const& options = {});

    Result<int> execute(LoadedProgram& program, FunctionHandle entry, ExecuteOptions const& options = {});
    Result<int> executeModule(artifact::Module const& module, ExecuteOptions const& options = {});
}
