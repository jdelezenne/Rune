#pragma once

#include "Rune/Parser/Attribute.hpp"

namespace rune::ecs
{
    Optional<parser::Attributes::Definition const*> findAttributeDefinition(StringView name);
}
