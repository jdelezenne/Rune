#pragma once

#include "Rune/Core/StringView.hpp"

namespace rune::sema
{
    namespace names
    {
        inline constexpr StringView iterator = "Iterator";
        inline constexpr StringView iterable = "Iterable";
        inline constexpr StringView allocator = "Allocator";
        inline constexpr StringView systemAllocator = "SystemAllocator";
        inline constexpr StringView collection = "Collection";

        inline constexpr StringView mutableCollection = "MutableCollection";
        inline constexpr StringView rangeReplaceableCollection = "RangeReplaceableCollection";
        inline constexpr StringView set = "Set";
        inline constexpr StringView dictionary = "Dictionary";
        inline constexpr StringView nilLiteralConvertible = "NilLiteralConvertible";
        inline constexpr StringView booleanLiteralConvertible = "BooleanLiteralConvertible";
        inline constexpr StringView integerLiteralConvertible = "IntegerLiteralConvertible";
        inline constexpr StringView floatingPointLiteralConvertible = "FloatingPointLiteralConvertible";
        inline constexpr StringView characterLiteralConvertible = "CharacterLiteralConvertible";
        inline constexpr StringView stringLiteralConvertible = "StringLiteralConvertible";
        inline constexpr StringView arrayLiteralConvertible = "ArrayLiteralConvertible";
        inline constexpr StringView dictionaryLiteralConvertible = "DictionaryLiteralConvertible";
        inline constexpr StringView rangeLiteralConvertible = "RangeLiteralConvertible";
        inline constexpr StringView closedRangeLiteralConvertible = "ClosedRangeLiteralConvertible";
        inline constexpr StringView partialRangeFromLiteralConvertible = "PartialRangeFromLiteralConvertible";
        inline constexpr StringView partialRangeToLiteralConvertible = "PartialRangeToLiteralConvertible";
        inline constexpr StringView partialRangeThroughLiteralConvertible = "PartialRangeThroughLiteralConvertible";

        inline constexpr StringView iteratorAssociatedType = "Iter";
        inline constexpr StringView makeIterator = "makeIterator";
        inline constexpr StringView next = "next";
        inline constexpr StringView allocate = "allocate";
        inline constexpr StringView deallocate = "deallocate";
    }
}
