#pragma once

#include "Rune/Modules/Manifest/ModuleManifest.hpp"

namespace rune
{
    Optional<Platform::HostPlatform> readManifestPlatformDefault(String const& manifestPath);

    Result<ModuleManifest> loadModuleManifestJson(
        String const& manifestPath,
        Platform::HostPlatform targetPlatform);

    Result<Void> saveModuleManifestJson(ModuleManifest const& manifest, String const& manifestPath);
}
