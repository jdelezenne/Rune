#pragma once

#include "Rune/Core/BinaryReader.hpp"
#include "Rune/Core/BinaryWriter.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune::ril
{
    void writeType(BinaryWriter& writer, Type const& type);
    void writeStaticData(BinaryWriter& writer, StaticData const& data);
    void writeFunction(BinaryWriter& writer, Function const& function);

    bool readType(BinaryReader& reader, Type& type);
    bool readStaticData(BinaryReader& reader, StaticData& data);
    bool readFunction(BinaryReader& reader, Function& function);
}
