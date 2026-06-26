#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

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
            Array<String> identifiers;
            HashMap<String, ID> index;
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
