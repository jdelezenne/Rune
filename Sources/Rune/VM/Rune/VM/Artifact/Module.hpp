#pragma once

#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/VM/Artifact/Format.hpp"
#include "Rune/VM/Artifact/Instruction.hpp"

namespace rune::vm::artifact
{
    enum class ValueStorageKind : uint8
    {
        Void,
        Scalar,
        Pointer,
        Address,
        Aggregate,
    };

    enum class ScalarKind : uint8
    {
        None,
        Integer,
        Float,
        Bool,
    };

    enum class CallableRepresentation : uint8
    {
        None,
        CFunctionPointer,
        RuneClosure,
    };

    struct TypeRecord final
    {
        ValueStorageKind storageKind{ValueStorageKind::Void};
        ScalarKind scalarKind{ScalarKind::None};
        uint32 byteSize{0};
        uint32 alignment{1};
        bool isSigned{false};
        CallableRepresentation callableRepresentation{CallableRepresentation::None};
        uint16 bitWidth{0};
        Array<TypeIndex> fields;
    };

    struct ConstantRecord final
    {
        TypeIndex type{invalidIndex};
        uint64 integer{0};
        double floating{0.0};
        bool boolean{false};
        String stringValue;
        Array<uint8> bytes;
        uint32 staticData{invalidIndex};
        uint32 staticDataOffset{0};
        uint32 byteLength{0};
    };

    struct StaticDataRecord final
    {
        struct Relocation final
        {
            uint32 offset{0};
            uint32 targetSlot{invalidIndex};
            uint32 targetOffset{0};
        };

        String name;
        Array<uint8> bytes;
        Array<Relocation> relocations;
        uint32 alignment{1};
    };

    struct LayoutFieldRecord final
    {
        LayoutIndex layout{invalidIndex};
        uint32 byteOffset{0};
    };

    struct LayoutRecord final
    {
        TypeIndex type{invalidIndex};
        uint32 byteSize{0};
        uint32 alignment{1};
        uint32 fieldStart{0};
        uint32 fieldCount{0};
        bool triviallyCopyable{true};
        bool triviallyDestroyable{true};
    };

    struct ValueSlotRecord final
    {
        TypeIndex type{invalidIndex};
        LayoutIndex layout{invalidIndex};
        uint32 byteOffset{0};
    };

    struct LocalRecord final
    {
        TypeIndex type{invalidIndex};
        LayoutIndex layout{invalidIndex};
        uint32 byteOffset{0};
        bool requiresInitialization{false};
    };

    struct BlockRecord final
    {
        InstructionIndex instructionStart{0};
        uint32 instructionCount{0};
    };

    struct FunctionRecord final
    {
        String name;
        TypeIndex returnType{invalidIndex};
        uint32 parameterStart{0};
        uint32 parameterCount{0};
        uint32 localStart{0};
        uint32 localCount{0};
        uint32 valueSlotStart{0};
        uint32 valueSlotCount{0};
        uint32 blockStart{0};
        uint32 blockCount{0};
        uint32 frameBytes{0};
        uint32 frameAlignment{1};
        bool isEntryPoint{false};
        bool isExternal{false};
        bool isModuleInitializer{false};
    };

    struct GlobalRecord final
    {
        String name;
        TypeIndex type{invalidIndex};
        LayoutIndex layout{invalidIndex};
        ConstantIndex initializer{invalidIndex};
        uint32 byteOffset{0};
        bool isMutable{false};
    };

    // Most @runtime bindings lower to a uniform foreign call dispatched by symbol
    // name. Operations listed here are answered by the interpreter itself from
    // the loaded program - no C symbol and no registration step. Values mirror
    // ril::RuntimeOperation.
    enum class RuntimeOperation : uint8
    {
        None,
        IntrospectionDataCount,
        IntrospectionDataAddress,
        IntrospectionDataLength,
    };

    struct AbiRecord final
    {
        String symbolName;
        RuntimeOperation runtimeOperation{RuntimeOperation::None};
        TypeIndex returnType{invalidIndex};
        uint32 parameterStart{0};
        uint32 parameterCount{0};
    };

#pragma region Debug Records

    struct DebugSourceFileRecord final
    {
        String filePath;
    };

    struct DebugInstructionRecord final
    {
        FunctionIndex function{invalidIndex};
        BlockIndex block{invalidIndex};
        uint32 instructionOffset{0};
        uint32 sourceFile{invalidIndex};
        uint32 line{0};
        uint32 column{0};
    };

    enum class DebugLocalPresentationKind : uint8
    {
        Default,
        ScalarBuiltin,
        IndexedElements,
        NullablePointer,
    };

    struct DebugLocalRecord final
    {
        FunctionIndex function{invalidIndex};
        LocalIndex local{invalidIndex};
        String name;
        String typeName;
        uint32 fieldStart{0};
        uint32 fieldCount{0};
        DebugLocalPresentationKind presentationKind{DebugLocalPresentationKind::Default};
        TypeIndex elementType{invalidIndex};
        String elementTypeName;
        uint32 dataOffset{0};
        uint32 countOffset{0};
        uint32 elementStride{0};
        bool dataIsInline{false};
        uint32 visibleAfterSourceFile{invalidIndex};
        uint32 visibleAfterLine{0};
    };

    struct DebugLocalFieldRecord final
    {
        String name;
        TypeIndex type{invalidIndex};
        uint32 byteOffset{0};
        String typeName;
        uint32 fieldStart{0};
        uint32 fieldCount{0};
        DebugLocalPresentationKind presentationKind{DebugLocalPresentationKind::Default};
        TypeIndex elementType{invalidIndex};
        String elementTypeName;
        uint32 dataOffset{0};
        uint32 countOffset{0};
        uint32 elementStride{0};
        bool dataIsInline{false};
    };

#pragma endregion

    struct Module final
    {
        String name;
        bool hasEntryPoint{false};
        SummaryRecord summary;
        Array<TypeRecord> types;
        Array<ConstantRecord> constants;
        Array<StaticDataRecord> staticData;
        Array<LayoutRecord> layouts;
        Array<LayoutFieldRecord> layoutFields;
        Array<FunctionRecord> functions;
        Array<ValueSlotRecord> valueSlots;
        Array<LocalRecord> locals;
        Array<BlockRecord> blocks;
        Array<Instruction> instructions;
        Array<GlobalRecord> globals;
        Array<AbiRecord> abi;

        Array<DebugSourceFileRecord> debugSourceFiles;
        Array<DebugInstructionRecord> debugInstructions;
        Array<DebugLocalRecord> debugLocals;
        Array<DebugLocalFieldRecord> debugLocalFields;

        /// Program introspection metadata (`sema::encodeIntrospectionRecords`
        /// encoding). The interpreter serves it to the Introspection module
        /// directly (RuntimeOperation::IntrospectionData*); every artifact
        /// persists its metadata.
        Array<uint8> introspection;
    };

    Result<Array<uint8>> encodeModuleArtifact(Module const& module);
    Result<Module> decodeModuleArtifact(Array<uint8> const& bytes, String const& artifactPath = {});
    Result<bool> writeModuleArtifact(Module const& module, String const& path);
    Result<Module> readModuleArtifact(String const& path);
}
