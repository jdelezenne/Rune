#pragma once

#include "Rune/Core/StringView.hpp"

namespace rune::ril::names
{
    inline constexpr StringView entryBlock = "entry";
    inline constexpr StringView mainFunction = "main";

    inline constexpr StringView ifThenBlock = "if.then";
    inline constexpr StringView ifElseBlock = "if.else";
    inline constexpr StringView ifEndBlock = "if.end";
    inline constexpr StringView guardThenBlock = "guard.then";
    inline constexpr StringView guardElseBlock = "guard.else";
    inline constexpr StringView whileConditionBlock = "while.condition";
    inline constexpr StringView whileBodyBlock = "while.body";
    inline constexpr StringView whileEndBlock = "while.end";
    inline constexpr StringView loopBodyBlock = "loop.body";
    inline constexpr StringView loopEndBlock = "loop.end";
    inline constexpr StringView forConditionBlock = "for.condition";
    inline constexpr StringView forBodyBlock = "for.body";
    inline constexpr StringView forEndBlock = "for.end";
    inline constexpr StringView forSkipInitBlock = "for.skip.init";
    inline constexpr StringView forSkipConditionBlock = "for.skip.condition";
    inline constexpr StringView forSkipNextBlock = "for.skip.next";
    inline constexpr StringView forSkipDecrementBlock = "for.skip.decrement";
    inline constexpr StringView forRangeLocal = "for.range";
    inline constexpr StringView forIteratorLocal = "for.iterator";
    inline constexpr StringView forStepLocal = "for.step";
    inline constexpr StringView forNextPointerLocal = "for.next.pointer";
    inline constexpr StringView forNextValueLocal = "for.next.value";
    inline constexpr StringView forSkipRemainingLocal = "for.skip.remaining";
    inline constexpr StringView forSkipValueLocal = "for.skip.value";

    inline constexpr StringView arrayLiteralLocal = "array.literal";
    inline constexpr StringView arrayLiteralElementLocal = "array.literal.element";
    inline constexpr StringView arrayLiteralBufferLocal = "array.literal.buffer";
    inline constexpr StringView inlineArrayLiteralLocal = "inlinearray.literal";
    inline constexpr StringView sliceObjectLocal = "slice.object";
    inline constexpr StringView intrinsicSelfLocal = "intrinsic.self";
    inline constexpr StringView intrinsicArgumentLocal = "intrinsic.argument";
    inline constexpr StringView methodReceiverTemporaryLocal = "receiver.temporary";
    inline constexpr StringView tupleDestructureLocal = "tuple.destructure";
    inline constexpr StringView stringAddLeftLocal = "string.add.left";
    inline constexpr StringView stringAddRightLocal = "string.add.right";
    inline constexpr StringView stringEqualLeftLocal = "string.equal.left";
    inline constexpr StringView stringEqualRightLocal = "string.equal.right";
    inline constexpr StringView matchValueLocal = "match.value";
    inline constexpr StringView matchStringValueLocal = "match.string.value";
    inline constexpr StringView matchStringPatternLocal = "match.string.pattern";
    inline constexpr StringView matchEndBlock = "match.end";
    inline constexpr StringView matchCaseBlock = "match.case";
    inline constexpr StringView matchNextBlock = "match.next";
    inline constexpr StringView optionReceiverLocal = "option.receiver";
    inline constexpr StringView optionBindingLocal = "option.binding";
    inline constexpr StringView optionUnwrapOrLocal = "option.unwrapOr";
    inline constexpr StringView optionUnwrapOrSomeBlock = "option.unwrapOr.some";
    inline constexpr StringView optionUnwrapOrNoneBlock = "option.unwrapOr.none";
    inline constexpr StringView optionUnwrapOrEndBlock = "option.unwrapOr.end";
    inline constexpr StringView optionCoalesceValueLocal = "option.coalesce.value";
    inline constexpr StringView optionCoalesceResultLocal = "option.coalesce.result";
    inline constexpr StringView optionCoalesceSomeBlock = "option.coalesce.some";
    inline constexpr StringView optionCoalesceNoneBlock = "option.coalesce.none";
    inline constexpr StringView optionCoalesceEndBlock = "option.coalesce.end";
    inline constexpr StringView ternaryResultLocal = "ternary.result";
    inline constexpr StringView ternaryTrueBlock = "ternary.true";
    inline constexpr StringView ternaryFalseBlock = "ternary.false";
    inline constexpr StringView ternaryEndBlock = "ternary.end";
    inline constexpr StringView shortCircuitResultLocal = "shortCircuit.result";
    inline constexpr StringView shortCircuitRightBlock = "shortCircuit.right";
    inline constexpr StringView shortCircuitEndBlock = "shortCircuit.end";
    inline constexpr StringView optionForceLocal = "option.force";

    inline constexpr StringView arrayType = "Array";
    inline constexpr StringView bufferType = "Buffer";
    inline constexpr StringView arrayIteratorType = "ArrayIterator";
    inline constexpr StringView inlineArrayType = "InlineArray";
    inline constexpr StringView inlineArrayIteratorType = "InlineArrayIterator";
    inline constexpr StringView memoryType = "Memory";
    inline constexpr StringView sliceType = "Slice";
    inline constexpr StringView rangeType = "Range";
    inline constexpr StringView rangeInclusiveType = "RangeInclusive";

    inline constexpr StringView initializeMember = "initialize";
    inline constexpr StringView countMember = "count";
    inline constexpr StringView reserveMember = "reserve";
    inline constexpr StringView appendMember = "append";
    inline constexpr StringView currentAllocatorMember = "currentAllocator";
    inline constexpr StringView initializeDefaultAllocatorMember = "initializeDefaultAllocator";
    inline constexpr StringView isEmptyMember = "isEmpty";
    inline constexpr StringView getAtMember = "getAt";
    inline constexpr StringView setAtMember = "setAt";
    inline constexpr StringView makeIteratorMember = "makeIterator";
    inline constexpr StringView nextMember = "next";
    inline constexpr StringView nextPointerMember = "nextPointer";
    inline constexpr StringView subscriptMember = "[]";

    inline constexpr StringView runeConsolePrint = "rune_console_print";
    inline constexpr StringView runePanicRune = "rune_panic_rune";
    inline constexpr StringView runeAssertionFailureRune = "rune_assertion_failure_rune";
    inline constexpr StringView runeInlineArrayElementPtr = "rune_inlinearray_element_ptr";

    inline constexpr StringView initializerFunction = "init";
    inline constexpr StringView foreignOptionArgumentLocal = "foreign.option.argument";
    inline constexpr StringView foreignPointerArgumentLocal = "foreign.pointer.argument";
    inline constexpr StringView foreignPointerSomeBlock = "foreign.pointer.some";
    inline constexpr StringView foreignPointerNoneBlock = "foreign.pointer.none";
    inline constexpr StringView foreignPointerEndBlock = "foreign.pointer.end";
    inline constexpr StringView foreignHandleOutLocal = "foreign.handle.out";
    inline constexpr StringView foreignHandleSomeBlock = "foreign.handle.some";
    inline constexpr StringView foreignHandleNoneBlock = "foreign.handle.none";
    inline constexpr StringView foreignHandleEndBlock = "foreign.handle.end";
    inline constexpr StringView foreignResultLocal = "foreign.result";
    inline constexpr StringView foreignOptionResultLocal = "foreign.option.result";
    inline constexpr StringView foreignOptionSomeBlock = "foreign.option.some";
    inline constexpr StringView foreignOptionNoneBlock = "foreign.option.none";
    inline constexpr StringView foreignOptionEndBlock = "foreign.option.end";
    inline constexpr StringView defaultAllocatorLocal = "default.allocator";
    inline constexpr StringView defaultAllocatorInterfaceLocal = "default.allocator.interface";
    inline constexpr StringView makeDefaultAllocateOkBlock = "make.default.allocate.ok";
    inline constexpr StringView makeDefaultAllocateErrorBlock = "make.default.allocate.error";
    inline constexpr StringView makeDefaultAllocateEndBlock = "make.default.allocate.end";
    inline constexpr StringView makeDefaultAllocateValueLocal = "make.default.allocate.value";

    inline constexpr StringView macroPrintValueLocal = "macro.print.value";
    inline constexpr StringView macroPanicMessageLocal = "macro.panic.message";
    inline constexpr StringView macroPanicFileLocal = "macro.panic.file";
    inline constexpr StringView macroAssertFailBlock = "macro.assert.fail";
    inline constexpr StringView macroAssertEndBlock = "macro.assert.end";
    inline constexpr StringView macroUnknownFile = "<unknown>";
    inline constexpr StringView macroPanicDefaultMessage = "panic";
    inline constexpr StringView macroAssertDefaultMessage = "assertion failed";
    inline constexpr StringView macroPrintLineBreak = "\n";
}
