#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

#include <functional>
#include <unordered_map>

namespace rune::sema
{
    namespace Identifiers
    {
        using ID = uint32;

        /// Uniqued identifier blob.
        class Table final
        {
        public:
            ID getOrInsert(StringView text);
            Optional<ID> find(StringView text) const;
            StringView lookup(ID id) const;
            Array<String> const& strings() const
            {
                return identifiers;
            }
            usize size() const
            {
                return identifiers.size();
            }

        private:
            struct TransparentStringHash final
            {
                using is_transparent = void;

                usize operator()(StringView text) const noexcept
                {
                    return std::hash<StringView>{}(text);
                }

                usize operator()(String const& text) const noexcept
                {
                    return std::hash<StringView>{}(StringView(text));
                }
            };

            struct TransparentStringEqual final
            {
                using is_transparent = void;

                bool operator()(StringView left, StringView right) const noexcept
                {
                    return left == right;
                }

                bool operator()(String const& left, StringView right) const noexcept
                {
                    return StringView(left) == right;
                }

                bool operator()(StringView left, String const& right) const noexcept
                {
                    return left == StringView(right);
                }

                bool operator()(String const& left, String const& right) const noexcept
                {
                    return left == right;
                }
            };

            Array<String> identifiers;
            std::unordered_map<String, ID, TransparentStringHash, TransparentStringEqual> index;
        };
    }
}

namespace rune::sema
{
    using IdentifierID = Identifiers::ID;
    using IdentifierTable = Identifiers::Table;
}

namespace rune
{
    using sema::IdentifierID;
    using sema::IdentifierTable;
}
