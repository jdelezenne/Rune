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
    /// Reads typed binary values from a byte buffer with bounds checking.
    class BinaryReader final
    {
    public:
        BinaryReader(Array<uint8> const& contents, usize cursor = 0);
        BinaryReader(uint8 const* data, usize size, usize cursor = 0);

        template <typename T>
        requires std::is_trivially_copyable_v<T>
        bool readPod(T& value)
        {
            if (!valid)
            {
                return false;
            }

            if (cursor + sizeof(T) > dataSize)
            {
                valid = false;
                return false;
            }

            std::memcpy(&value, dataPtr + cursor, sizeof(T));
            cursor += sizeof(T);
            return true;
        }

        template <typename T>
        requires std::is_integral_v<T>
        bool readInteger(uint64& value)
        {
            T encoded = {};
            if (!readPod(encoded))
            {
                return false;
            }

            if constexpr (std::is_signed_v<T>)
            {
                value = static_cast<uint64>(static_cast<int64>(encoded));
            }
            else
            {
                value = static_cast<uint64>(encoded);
            }

            return true;
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
        bool readPodArray(Array<T>& values)
        {
            uint32 count = 0;
            if (!readPod(count))
            {
                return false;
            }

            values.clear();
            values.reserve(count);
            for (uint32 index = 0; index < count; ++index)
            {
                T value{};

                if (!readPod(value))
                {
                    return false;
                }
                values.push_back(value);
            }

            return true;
        }

        bool readString(String& value);
        bool readStringArray(Array<String>& values);
        bool readBytes(StringView bytes);
        bool readBytes(Array<uint8>& bytes, usize count);
        bool readBoolArray(Array<bool>& values);
        bool readUsizeArray(Array<usize>& values);
        bool readUInt32Array(Array<uint32>& values);
        bool readStringUInt32Map(HashMap<String, uint32>& values);

        bool ok() const
        {
            return valid;
        }

        usize offset() const
        {
            return cursor;
        }

        bool eof() const
        {
            return cursor >= dataSize;
        }

        uint8 const* data() const
        {
            return dataPtr;
        }

        usize size() const
        {
            return dataSize;
        }

        explicit operator bool() const
        {
            return valid;
        }

    private:
        uint8 const* dataPtr;
        usize dataSize;
        usize cursor;
        bool valid;
    };

    template <typename T>
    requires std::is_trivially_copyable_v<T>
    bool readPod(Array<uint8> const& data, usize& offset, T& value)
    {
        BinaryReader reader(data, offset);
        if (!reader.readPod(value))
        {
            return false;
        }

        offset = reader.offset();
        return true;
    }

    bool readString(Array<uint8> const& data, usize& offset, String& value);
    bool readStringArray(Array<uint8> const& data, usize& offset, Array<String>& values);
    bool readBoolArray(Array<uint8> const& data, usize& offset, Array<bool>& values);
    bool readUsizeArray(Array<uint8> const& data, usize& offset, Array<usize>& values);
    bool readUInt32Array(Array<uint8> const& data, usize& offset, Array<uint32>& values);
    bool readStringUInt32Map(Array<uint8> const& data, usize& offset, HashMap<String, uint32>& values);
}
