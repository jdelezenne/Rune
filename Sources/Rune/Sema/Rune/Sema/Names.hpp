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
        inline constexpr StringView indexable = "Indexable";
        inline constexpr StringView mutableIndexable = "MutableIndexable";
        inline constexpr StringView collection = "Collection";
        inline constexpr StringView inlineArray = "InlineArray";
        inline constexpr StringView intType = "Int";
        inline constexpr StringView voidType = "Void";

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
        inline constexpr StringView mutableIteratorAssociatedType = "MutableIter";
        inline constexpr StringView immutableIteratorAssociatedType = "ImmutableIter";
        inline constexpr StringView makeIterator = "makeIterator";
        inline constexpr StringView makeMutableIterator = "makeMutableIterator";
        inline constexpr StringView makeImmutableIterator = "makeImmutableIterator";
        inline constexpr StringView initializer = "init";
        inline constexpr StringView next = "next";
        inline constexpr StringView subscriptOperator = "[]";
        inline constexpr StringView allocate = "allocate";
        inline constexpr StringView deallocate = "deallocate";
    }
}
