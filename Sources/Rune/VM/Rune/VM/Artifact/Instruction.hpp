#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Types.hpp"

namespace rune::vm::artifact
{
    using TypeIndex = uint32;
    using ConstantIndex = uint32;
    using LayoutIndex = uint32;
    using FunctionIndex = uint32;
    using GlobalIndex = uint32;
    using AbiIndex = uint32;
    using BlockIndex = uint32;
    using ValueSlot = uint32;
    using LocalIndex = uint32;
    using FieldIndex = uint32;
    using InstructionIndex = uint32;

    inline constexpr uint32 invalidIndex = 0xFFFF'FFFFu;

    enum class Opcode : uint16
    {
        Nop,
        Trap,
        ReturnVoid,
        Return,
        Branch,
        BranchIf,

        Constant,
        Null,
        FunctionReference,
        WitnessTableReference,

        LocalLoad,
        LocalAddress,
        LocalInitialize,
        LocalAssign,
        GlobalLoad,
        GlobalAddress,
        GlobalInitialize,
        GlobalAssign,

        AddressField,
        Load,
        Store,
        Copy,
        Move,
        Destroy,
        PointerLoad,
        PointerNonNull,

        AggregateMake,
        AggregateGet,
        AggregateSet,
        TupleMake,
        TupleGet,
        UnionMake,
        UnionTag,
        UnionPayload,

        Binary,
        Unary,
        Cast,

        Call,
        CallIndirect,
        CallWitness,
        ClosureMake,
        CallbackMake,

        ForeignCall,
        CompilerIntrinsic,

        DynamicTypeMatch,
        AnyBox,
        AnyExtract,
    };

    enum class BinaryOp : uint8
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Equal,
        NotEqual,
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual,
        LogicalAnd,
        LogicalOr,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        LeftShift,
        RightShift,
    };

    enum class UnaryOp : uint8
    {
        Plus,
        Negate,
        LogicalNot,
        BitwiseNot,
    };

    enum class CastPolicy : uint8
    {
        Checked,
        Truncating,
        Clamping,
    };

    enum class FunctionReferenceRepresentation : uint8
    {
        RuneFunctionHandle,
        CFunctionPointer,
        RuneClosure,
    };

    struct Instruction final
    {
        Opcode opcode{Opcode::Nop};
        ValueSlot destination{invalidIndex};
        ValueSlot operand0{invalidIndex};
        ValueSlot operand1{invalidIndex};
        ValueSlot operand2{invalidIndex};
        uint32 immediate0{0};
        uint32 immediate1{0};
        LayoutIndex layout{invalidIndex};
        AbiIndex abi{invalidIndex};
        FunctionIndex function{invalidIndex};
        BlockIndex block0{invalidIndex};
        BlockIndex block1{invalidIndex};
        BinaryOp binaryOp{BinaryOp::Add};
        UnaryOp unaryOp{UnaryOp::Plus};
        CastPolicy castPolicy{CastPolicy::Checked};
        Array<ValueSlot> operands;
    };
}
