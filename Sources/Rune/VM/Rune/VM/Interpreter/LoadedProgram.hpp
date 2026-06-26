#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/VM/Artifact/Module.hpp"

namespace rune::vm::interpreter
{
    using ModuleHandle = uint32;
    using FunctionHandle = uint32;
    using BlockHandle = uint32;
    using LayoutHandle = uint32;
    using GlobalHandle = uint32;
    using AbiHandle = uint32;

    struct LoadedInstruction final
    {
        ModuleHandle module{artifact::invalidIndex};
        artifact::InstructionIndex artifactInstruction{artifact::invalidIndex};
        artifact::Instruction instruction;
        FunctionHandle function{artifact::invalidIndex};
        GlobalHandle global{artifact::invalidIndex};
        AbiHandle abi{artifact::invalidIndex};
    };

    struct LoadedFunction final
    {
        ModuleHandle module{artifact::invalidIndex};
        artifact::FunctionIndex artifactFunction{artifact::invalidIndex};
        uint32 instructionStart{0};
        uint32 instructionCount{0};
        uint32 blockStart{0};
        uint32 blockCount{0};
        uint32 valueSlotStart{0};
        uint32 parameterCount{0};
        uint32 localCount{0};
        uint32 frameBytes{0};
        uint32 valueSlotCount{0};
        bool isEntryPoint{false};
        bool isModuleInitializer{false};
    };

    struct LoadedBlock final
    {
        FunctionHandle function{artifact::invalidIndex};
        uint32 instructionStart{0};
        uint32 instructionCount{0};
    };

    struct LoadedModule final
    {
        String name;
        artifact::Module const* artifact{nullptr};
        uint32 functionStart{0};
        uint32 functionCount{0};
        uint32 globalStart{0};
        uint32 globalCount{0};
        uint32 abiStart{0};
        uint32 abiCount{0};
    };

    struct LoadedGlobal final
    {
        ModuleHandle module{artifact::invalidIndex};
        artifact::GlobalIndex artifactGlobal{artifact::invalidIndex};
        uint32 byteOffset{0};
        uint32 byteSize{0};
        uint32 alignment{1};
        bool isMutable{false};
    };

    struct LoadedAbi final
    {
        ModuleHandle module{artifact::invalidIndex};
        artifact::AbiIndex artifactAbi{artifact::invalidIndex};
        String symbolName;
        artifact::RuntimeOperation runtimeOperation{artifact::RuntimeOperation::None};
    };

    struct LoadedProgram final
    {
        Array<artifact::Module> artifacts;
        Array<LoadedModule> modules;
        Array<LoadedFunction> functions;
        Array<LoadedBlock> blocks;
        Array<LoadedGlobal> globals;
        Array<LoadedAbi> abi;
        Array<LoadedInstruction> instructions;
        Array<uint8> globalStorage;
        Array<FunctionHandle> moduleInitializers;
        FunctionHandle entryFunction{artifact::invalidIndex};
        bool moduleInitializersExecuted{false};
    };

    Result<LoadedProgram> loadProgram(Array<artifact::Module> modules);
}
