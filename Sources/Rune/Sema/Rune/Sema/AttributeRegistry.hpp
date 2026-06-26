#pragma once

#include "Rune/Parser/Attribute.hpp"

namespace rune::sema
{
    using AttributeTarget = parser::Attributes::Target;
    using AttributeArgumentPolicy = parser::Attributes::ArgumentPolicy;
    using AttributeDefinition = parser::Attributes::Definition;

    inline StringView attributeTargetDisplayName(AttributeTarget target)
    {
        return parser::Attributes::targetDisplayName(target);
    }

    constexpr bool hasAttributeTarget(AttributeTarget allowed, AttributeTarget target)
    {
        return parser::Attributes::hasFlag(allowed, target);
    }

    inline Optional<AttributeDefinition const*> findCompilerAttributeDefinition(StringView name)
    {
        return parser::Attributes::findDefinition(name);
    }

    inline bool attributeCanApplyDirectlyToTarget(StringView name, AttributeTarget target)
    {
        return parser::Attributes::canApplyDirectlyToTarget(name, target);
    }
}
