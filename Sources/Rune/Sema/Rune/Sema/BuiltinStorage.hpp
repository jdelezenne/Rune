#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

namespace rune
{
    using BuiltinStorageSizeFn = usize (*)();
    using BuiltinStorageAlignmentFn = usize (*)();
    using BuiltinStorageConstructFn = void (*)(void* storage);
    using BuiltinStorageDestructFn = void (*)(void* storage);

    struct BuiltinTypeLayout final
    {
        char const* typeName;
        BuiltinStorageSizeFn storageSize;
        BuiltinStorageAlignmentFn storageAlignment;
        BuiltinStorageConstructFn construct;
        BuiltinStorageDestructFn destruct;
    };

    enum class BuiltinRuntimeField
    {
        RangeStart,
        RangeEnd,
        RangeVariant,
        RangeIteratorCurrent,
        RangeIteratorEnd,
        RangeIteratorVariant,
        RangeIteratorExhausted,
        StringAllocator,
        StringData,
        StringLength,
        StringCapacity,
        BufferElementSize,
        BufferData,
        BufferCapacity,
        ArrayElementSize,
        ArrayData,
        ArrayCount,
        ArrayCapacity,
        SliceData,
        SliceCount,
        InlineArrayData,
        InlineArrayCount,
        InlineArrayStride,
    };

    /// C runtime type names used for built-in struct storage layout.

    BuiltinTypeLayout const* findBuiltinTypeLayout(StringView typeName);

    Optional<String> builtinRuntimeStorageNameForSourceType(StringView typeName);

    usize builtinTypeStorageSize(StringView typeName);

    usize builtinTypeStorageAlignment(StringView typeName);

    BuiltinStorageConstructFn builtinTypeConstruct(StringView typeName);

    BuiltinStorageDestructFn builtinTypeDestruct(StringView typeName);

    Optional<uint32> builtinRuntimeFieldOffset(StringView typeName, BuiltinRuntimeField field);

    Optional<uint32> builtinRuntimeFieldSize(StringView typeName, BuiltinRuntimeField field);
}
