#pragma once

#include "Rune/Common/BackendInterface.hpp"

namespace rune
{
    struct CBackend final : BackendInterface
    {
        bool isNative() const override;
        Result<bool> emit(BackendEmitContext& ctx) override;
        int execute(BackendExecuteContext const& ctx) override;
    };
}
