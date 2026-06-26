#pragma once

#include "Rune/Core/CommandLineParser.hpp"
#include "Rune/Modules/Manifest/ModuleManifest.hpp"

namespace rune
{
    bool validateModuleManifest(
        String const& manifestPath,
        ModuleManifestLoadOptions const& loadOptions = {});

    bool writeModuleManifest(CommandLineArguments const& arguments);
}
