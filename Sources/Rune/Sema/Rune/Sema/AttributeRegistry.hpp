#pragma once

#include "Rune/Parser/Attribute.hpp"

#if RUNE_ENABLE_ECS
#include "Rune/ECS/Attributes.hpp"
#endif

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
#if RUNE_ENABLE_ECS
        Optional<AttributeDefinition const*> ecsDefinition = ecs::findAttributeDefinition(name);
        if (ecsDefinition.has_value())
        {
            return ecsDefinition;
        }
#endif

        return parser::Attributes::findDefinition(name);
    }

    inline bool attributeCanApplyDirectlyToTarget(StringView name, AttributeTarget target)
    {
        Optional<AttributeDefinition const*> definition = findCompilerAttributeDefinition(name);
        if (!definition.has_value())
        {
            return false;
        }

        return hasAttributeTarget((*definition)->allowedTargets, target);
    }
}
