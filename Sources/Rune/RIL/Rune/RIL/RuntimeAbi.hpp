#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/RIL/Primitives.hpp"
#include "Rune/Sema/ASTContext.hpp"
#include "Rune/Sema/CompilerKnownTypes.hpp"

namespace rune::ril
{
    enum class RuntimeAbiTypeKind
    {
        None,
        String,
        StringSlice,
        Buffer,
        Array,
        RuntimeStorage,
        Slice,
        InlineArray,
        ContiguousIterator,
        Range,
        RangeIterator,
    };

    struct RuntimeAbiType final
    {
        RuntimeAbiTypeKind kind{RuntimeAbiTypeKind::None};
        Optional<RangeKind> rangeKind;
        String runtimeStorageTypeName;
    };

    enum class RuntimeAbiField
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
        BufferData,
        BufferCapacity,
        ArrayData,
        ArrayCount,
        ArrayCapacity,
        SliceData,
        SliceCount,
        InlineArrayData,
        InlineArrayCount,
        InlineArrayStride,
    };

    struct RuntimeAbiLayout final
    {
        uint32 byteSize{0};
        uint32 alignment{1};
    };

    RuntimeAbiType runtimeAbiType(sema::KnownStandardTypeKind kind);
    RuntimeAbiType runtimeAbiType(ASTContext const& context, TypeID typeId);
    RuntimeAbiType runtimeAbiType(Type const& type);
    Optional<RuntimeAbiLayout> runtimeAbiLayout(RuntimeAbiType const& type);
    Optional<RuntimeAbiLayout> runtimeAbiLayout(RuntimeAbiTypeKind kind);
    Optional<uint32> runtimeAbiFieldOffset(RuntimeAbiTypeKind kind, RuntimeAbiField field);
}
