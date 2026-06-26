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
        Array<Global> globals;
        Array<Function> functions;

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

        uint32 addFunction(Function function)
        {
            functions.push_back(std::move(function));
            return static_cast<uint32>(functions.size() - 1);
        }
    };

    Result<Constant> evaluateModuleConstant(Module const& module, uint32 value);
}
