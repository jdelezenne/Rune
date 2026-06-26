#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/BinaryReader.hpp"
#include "Rune/Core/BinaryWriter.hpp"
#include "Rune/Sema/TypeArena.hpp"

namespace rune::package
{
    Array<SemanticType> collectReferencedTypes(TypeArena const& types, Array<TypeID> const& roots);

    void writeTypeTable(BinaryWriter& writer, Array<SemanticType> const& types);

    bool readTypeTable(BinaryReader& reader, Array<SemanticType>& types);
}
