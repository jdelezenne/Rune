#pragma once
namespace rune
{
    namespace ASCII
    {
        /// Returns true when ch is an ASCII decimal digit.
        bool isDigit(char ch);

        /// Returns true when ch is an ASCII hexadecimal digit.
        bool isHexDigit(char ch);

        /// Returns true when ch is an ASCII binary digit.
        bool isBinaryDigit(char ch);

        /// Returns true when ch is an ASCII octal digit.
        bool isOctalDigit(char ch);

        /// Returns true when ch is an ASCII letter (a–z or A–Z).
        bool isAlpha(char ch);

        /// Returns true when ch is an ASCII letter or decimal digit.
        bool isAlphaNum(char ch);

        /// Returns true when ch is ASCII horizontal whitespace.
        bool isWhitespace(char ch);
    }
}
