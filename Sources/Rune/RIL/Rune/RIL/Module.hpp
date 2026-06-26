#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/Optional.hpp"
#include "Rune/Core/Result.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/RIL/Primitives.hpp"

#include <utility>

namespace rune::ril
{
    struct Local final
    {
        String name;
        Type type;
        bool requiresInitialization{true};
        bool isUserVisible{false};
        SourceLocation declarationLocation;
    };

    struct BasicBlock final
    {
        String name;
        Array<Statement> statements;
        Optional<Terminator> terminator;
    };

    struct Global final
    {
        String name;
        Type type;
        uint32 initializerValue{0};
        bool isMutable{false};
        bool requiresRuntimeInitialization{false};
    };

    struct StaticData final
    {
        struct Relocation final
        {
            uint32 offset{0};
            uint32 targetSlot{0};
            uint32 targetOffset{0};
        };

        String name;
        Array<uint8> bytes;
        Array<Relocation> relocations;
        uint32 alignment{1};
        Optional<Type> type;
    };

    struct Function final
    {
        String name;
        Type returnType;
        Array<Type> parameters;
        Array<Value> values;
        Array<Local> locals;
        Array<BasicBlock> blocks;
        bool isEntryPoint{false};
        bool isExternal{false};
        bool isModuleInitializer{false};
        bool isModuleLocal{false};
        Optional<SourceLocation> defaultValueLocation;

        uint32 addValue(Value value)
        {
            if (defaultValueLocation.has_value() &&
                value.location.line == 0 &&
                value.location.fileName.empty())
            {
                value.location = *defaultValueLocation;
            }

            values.push_back(std::move(value));
            return static_cast<uint32>(values.size() - 1);
        }

        uint32 addLocal(Local local)
        {
            locals.push_back(std::move(local));
            return static_cast<uint32>(locals.size() - 1);
        }
    };

    struct Module final
    {
        String name;
        Array<Value> values;
        Array<StaticData> staticData;
        Array<Global> globals;
        Array<Function> functions;
        /// C headers the module's manifest declares (or, for source builds, the
        /// CLI's `--include-header`). The C backend #includes these and, for Symbol
        /// FFI calls, trusts them to provide the extern prototype (see
        /// `Value::foreignHasCIncludeHeader`).
        Array<String> includeHeaders;
        /// Program introspection records (`sema::encodeIntrospectionRecords`),
        /// written by backends as the artifact's introspection metadata
        /// section. The host runtime serves it to the Introspection module
        /// directly; every artifact persists its metadata.
        Array<uint8> introspectionData;

        explicit Module(String moduleName = {})
            : name(std::move(moduleName))
        {
        }

        uint32 addValue(Value value)
        {
            values.push_back(std::move(value));
            return static_cast<uint32>(values.size() - 1);
        }

        uint32 addGlobal(Global global)
        {
            globals.push_back(std::move(global));
            return static_cast<uint32>(globals.size() - 1);
        }

        uint32 addStaticData(StaticData data)
        {
            staticData.push_back(std::move(data));
            return static_cast<uint32>(staticData.size() - 1);
        }

        uint32 addFunction(Function function)
        {
            functions.push_back(std::move(function));
            return static_cast<uint32>(functions.size() - 1);
        }
    };

    /// Classifies globals whose source initializer is executed by a module
    /// initializer function. Backends must emit zero-initialized storage for
    /// these globals instead of lowering their placeholder RIL constant.
    Array<bool> runtimeInitializedGlobals(Module const& module);

    /// Returns whether a folded initializer has the common static-storage
    /// representation required by every backend. String literals are special:
    /// their source value carries the readonly-data relocation.
    bool canEncodeStaticGlobalInitializer(
        Module const& module,
        uint32 initializerValue,
        Constant const& initializer);

    Result<Constant> evaluateModuleConstant(Module const& module, uint32 value);
}
