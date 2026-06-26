#pragma once

#include "Rune/Core/Assert.hpp"
#include "Rune/Core/Logging.hpp"
#include "Rune/Sema/ASTContext.hpp"

namespace rune::sema
{
    inline StringView semaTraceFlagName(SemaTraceFlag flag)
    {
        switch (flag)
        {
            case SemaTraceFlag::Pipeline:
                return "pipeline";

            case SemaTraceFlag::Declaration:
                return "decl";

            case SemaTraceFlag::Import:
                return "import";

            case SemaTraceFlag::Lookup:
                return "lookup";

            case SemaTraceFlag::Type:
                return "type";

            case SemaTraceFlag::None:
            case SemaTraceFlag::All:
                break;
        }

        RUNE_UNREACHABLE();
    }

    inline bool shouldTraceSema(ASTContext const& context, SemaTraceFlag flag)
    {
        return hasSemaTraceFlag(context.getSemaTraceFlags(), flag);
    }

    inline void traceSema(
        ASTContext const& context,
        SemaTraceFlag flag,
        StringView message)
    {
        if (!shouldTraceSema(context, flag))
        {
            return;
        }

        Logger::info(
            std::format(
                "Sema:{}:{}: {}",
                context.getModuleDecl().name,
                semaTraceFlagName(flag),
                message));
    }

    template <typename... Args>
    void traceSema(
        ASTContext const& context,
        SemaTraceFlag flag,
        StringView format,
        Args const&... args)
    {
        if (!shouldTraceSema(context, flag))
        {
            return;
        }

        traceSema(
            context,
            flag,
            String(std::vformat(
                StringView(format.data(), format.size()),
                std::make_format_args(args...))));
    }
}
