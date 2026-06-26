#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/VM/Artifact/Module.hpp"
#include "Rune/VM/Interpreter/Frame.hpp"

namespace rune::vm::interpreter
{
    Result<Frame> makeFrame(artifact::FunctionRecord const& function);
    Result<bool> initializeFrame(Frame& frame, artifact::FunctionRecord const& function);
    Result<bool> destroyFrame(Frame& frame, artifact::FunctionRecord const& function);
}
