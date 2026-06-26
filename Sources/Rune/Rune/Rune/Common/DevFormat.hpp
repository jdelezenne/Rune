#pragma once

#include "Rune/Core/Array.hpp"
#include "Rune/Core/String.hpp"
#include "Rune/Core/Types.hpp"
#include "Rune/Lexer/Token.hpp"
#include "Rune/Parser/AST.hpp"
#include "Rune/RIL/Module.hpp"

namespace rune
{
    enum class DevOutputFormat
    {
        Text,
        Json,
    };

    struct DevOutputOptions final
    {
        DevOutputFormat format{DevOutputFormat::Text};
        String outputPath;
    };

    struct DevTokenizedFile final
    {
        String path;
        Array<lexer::Token> tokens;
    };

    bool writeLexerText(Array<DevTokenizedFile> const& files, usize totalTokens, String const& outputPath);
    bool writeLexerJson(Array<DevTokenizedFile> const& files, usize totalTokens, String const& outputPath);
    bool writeParserText(parser::Program& program, String const& outputPath);
    bool writeParserJson(parser::Program const& program, String const& outputPath);
    bool writeRILText(ril::Module const& module, String const& outputPath);
    bool writeRILJson(ril::Module const& module, String const& outputPath);
}
