#pragma once

#include "Rune/Core/HashMap.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Module.hpp"
#include "Rune/RIL/Primitives.hpp"

namespace rune::ril
{
    enum class MaterializedValueKind : uint8
    {
        Void,
        Integer,
        Float,
        Bool,
        Text,
        Tuple,
        Aggregate,
        Array,
        Buffer,
        InlineArray,
        Storage,
    };

    struct MaterializedValue final
    {
        MaterializedValueKind kind{MaterializedValueKind::Void};
        Type type{Type::voidType()};
        uint64 integer{0};
        float64 floating{0.0};
        bool boolean{false};
        String text;
        Array<uint8> storage;
        Array<MaterializedValue> elements;

        static MaterializedValue integerValue(Type valueType, uint64 value)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Integer,
                .type = std::move(valueType),
                .integer = value,
            };
        }

        static MaterializedValue floatingValue(Type valueType, float64 value)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Float,
                .type = std::move(valueType),
                .floating = value,
            };
        }

        static MaterializedValue booleanValue(bool value)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Bool,
                .type = Type::boolType(),
                .boolean = value,
            };
        }

        static MaterializedValue textValue(Type valueType, String value)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Text,
                .type = std::move(valueType),
                .text = std::move(value),
            };
        }

        static MaterializedValue tupleValue(Type valueType, Array<MaterializedValue> elementValues)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Tuple,
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static MaterializedValue aggregateValue(Type valueType, Array<MaterializedValue> elementValues)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Aggregate,
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static MaterializedValue arrayValue(Type valueType, Array<MaterializedValue> elementValues)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Array,
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static MaterializedValue bufferValue(Type valueType, Array<MaterializedValue> elementValues)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Buffer,
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static MaterializedValue inlineArrayValue(Type valueType, Array<MaterializedValue> elementValues)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::InlineArray,
                .type = std::move(valueType),
                .elements = std::move(elementValues),
            };
        }

        static MaterializedValue storageValue(Type valueType, Array<uint8> bytes)
        {
            return MaterializedValue{
                .kind = MaterializedValueKind::Storage,
                .type = std::move(valueType),
                .storage = std::move(bytes),
            };
        }
    };

    struct MaterializedStaticObject final
    {
        Type type{Type::voidType()};
        Array<StaticData> staticData;
        uint32 rootStaticDataSlot{0};
    };

    using MaterializedEvalValues = HashMap<parser::EvalExpression const*, MaterializedStaticObject>;

    struct EvaluationModule final
    {
        Module module;
        uint32 functionIndex{0};
    };
}
