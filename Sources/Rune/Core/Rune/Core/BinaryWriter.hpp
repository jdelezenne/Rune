#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/HashMap.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/StringView.hpp"
#include "Rune/Core/Types.hpp"

#include <concepts>
#include <cstring>
#include <type_traits>

namespace rune
{
    /// Appends typed binary values to a byte buffer.
    class BinaryWriter final
    {
    public:
        explicit BinaryWriter(Array<uint8>& storage)
            : storage(storage)
        {
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
        void writePod(T const& value)
        {
            usize const start = storage.size();
            storage.resize(start + sizeof(T));
            std::memcpy(storage.data() + start, &value, sizeof(T));
        }

        template <typename T>
        requires std::is_integral_v<T>
        void writeInteger(uint64 value)
        {
            T const encoded = static_cast<T>(value);
            writePod(encoded);
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
        void writePodArray(Array<T> const& values)
        {
            writePod<uint32>(static_cast<uint32>(values.size()));
            for (T const& value : values)
            {
                writePod(value);
            }
        }

        void writeString(String const& value);
        void writeString(StringView value);
        void writeStringArray(Array<String> const& values);
        void writeBoolArray(Array<bool> const& values);
        void writeUsizeArray(Array<usize> const& values);
        void writeUInt32Array(Array<uint32> const& values);
        void writeStringUInt32Map(HashMap<String, uint32> const& values);
        void writeBytes(StringView bytes);
        void writeBytes(Array<uint8> const& bytes);

        Array<uint8>& buffer()
        {
            return storage;
        }

        Array<uint8> const& buffer() const
        {
            return storage;
        }

    private:
        Array<uint8>& storage;
    };

    template <typename T>
    requires std::is_trivially_copyable_v<T>
    void appendPod(Array<uint8>& data, T const& value)
    {
        BinaryWriter(data).writePod(value);
    }

    void appendString(Array<uint8>& data, String const& value);
    void appendStringArray(Array<uint8>& data, Array<String> const& values);
    void appendBoolArray(Array<uint8>& data, Array<bool> const& values);
    void appendUsizeArray(Array<uint8>& data, Array<usize> const& values);
    void appendUInt32Array(Array<uint8>& data, Array<uint32> const& values);
    void appendStringUInt32Map(Array<uint8>& data, HashMap<String, uint32> const& values);
}
