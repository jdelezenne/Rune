#pragma once

#include "Rune/Core/Enum.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/StringView.hpp"

namespace rune::parser
{
    namespace Attributes
    {
        /// Declaration kinds an attribute may be attached to.
        enum class Target : uint16
        {
            None = 0,                  ///< No target (invalid/default).
            Function = 1 << 0,         ///< Function declarations.
            Variable = 1 << 1,         ///< Variable declarations.
            Struct = 1 << 2,           ///< Struct declarations.
            Property = 1 << 3,         ///< Property declarations.
            PropertyAccessor = 1 << 4, ///< Property accessors (get/set).
            StaticScope = 1 << 5,      ///< Static scope declarations.
            Extension = 1 << 6,        ///< Extension declarations.
            Enum = 1 << 7,             ///< Enum declarations.
            Union = 1 << 8,            ///< Union declarations.
            Interface = 1 << 9,        ///< Interface declarations.
            Alias = 1 << 10,           ///< Type alias declarations.
            DistinctType = 1 << 11,    ///< Distinct type declarations.
            AttributedGroup = 1 << 12, ///< Attributed declaration groups.
        };

        RUNE_DEFINE_ENUM_BITWISE_OPERATORS(Target, uint16)

        /// Gets a human-readable display name for the target.
        /// @param target The target.
        /// @return The display name.
        StringView targetDisplayName(Target target);

        /// How many arguments a compiler-known attribute accepts.
        enum class ArgumentPolicy : uint8
        {
            ExactlyZero, ///< Exactly zero arguments.
            AtMostOne,   ///< Zero or one argument.
            AtLeastOne,  ///< One or more arguments.
            ExactlyOne,  ///< Exactly one argument.
        };

        /// Represents the definition of a compiler-known attribute.
        struct Definition final
        {
            StringView name;               ///< The name of the attribute.
            ArgumentPolicy argumentPolicy; ///< The argument policy for the attribute.
            Target allowedTargets;         ///< Targets this attribute can be applied to.
            Target inheritedTargets;       ///< Targets this attribute is inherited by.
            Target groupTargets;           ///< Targets within an attributed group this attribute propagates to.
        };

        /// Finds the definition for a compiler-known attribute by name.
        /// @param name The attribute name to look up.
        /// @return The attribute definition if found, std::nullopt otherwise.
        Optional<Definition const*> findDefinition(StringView name);

        /// Checks if an attribute can appear on an attributed declaration group.
        /// @param name The attribute name.
        /// @return True if the attribute is group-expandable.
        bool canAppearOnGroup(StringView name);

        /// Checks if a group attribute inherits to a specific member target kind.
        /// @param name The attribute name.
        /// @param target The member target kind.
        /// @return True if the attribute inherits.
        bool canInheritToTarget(StringView name, Target target);

        /// Checks if an attribute can be applied directly to a target.
        /// @param name The attribute name.
        /// @param target The target kind.
        /// @return True if the attribute is allowed directly on the target.
        bool canApplyDirectlyToTarget(StringView name, Target target);
    }
}
