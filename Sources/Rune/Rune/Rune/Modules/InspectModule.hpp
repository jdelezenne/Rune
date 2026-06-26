#pragma once

#include "Rune/Sema/ASTContext.hpp"

namespace rune
{
    enum class InspectOutputFormat
    {
        Text,
        Json,
    };

    struct InspectModuleOptions final
    {
        InspectOutputFormat format{InspectOutputFormat::Text};
        String outputPath;
    };

    bool inspectModule(String const& modulePath, InspectModuleOptions const& options = {});
    bool inspectBytecode(String const& bytecodePath, InspectModuleOptions const& options = {});
    bool writeModuleText(Module const& module, String const& outputPath);
    bool writeModuleJson(Module const& module, String const& outputPath);
}
