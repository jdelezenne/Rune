#pragma once

#include <rune/allocator.h>
#include <rune/array.h>
#include <rune/forward.h>
#include <rune/memory.h>
#include <rune/stringslice.h>
#include <rune/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Owned, growable UTF-8 string.
///
/// `length` is the number of valid bytes in `data`. The buffer is not null-terminated.
typedef struct RuneString
{
    RuneAllocator const* allocator; ///< Allocator used for heap storage.
    char* data;                     ///< UTF-8 bytes, or null when empty.
    usize length;                   ///< Byte length of valid data in `data`.
    usize capacity;                 ///< Allocated byte capacity of `data`.
} RuneString;

/// Iterator over Unicode scalar values in a RuneString.
typedef struct RuneStringIterator
{
    char const* data; ///< UTF-8 bytes being iterated.
    usize length;     ///< Byte length of the view.
    usize offset;     ///< Current byte offset.
} RuneStringIterator;

/// @name Lifecycle
/// @{

/// Creates an empty RuneString using the specified allocator.
/// @param allocator Allocator used for heap storage.
/// @return The created empty RuneString.
RuneString rune_string_create();

/// Creates an empty RuneString using the specified allocator.
/// @param allocator Allocator used for heap storage.
/// @return The created empty RuneString.
RuneString rune_string_create_with_allocator(RuneAllocator const* allocator);

/// Creates a RuneString by copying the specified byte buffer.
/// @param allocator Allocator used for heap storage.
/// @param bytes Pointer to the byte buffer.
/// @param length Length of the byte buffer in bytes.
/// @return The constructed RuneString.
RuneString rune_string_from_bytes(RuneAllocator const* allocator, char const* bytes, usize length);

/// Deallocates the backing memory of the RuneString.
/// @param storage Pointer to the RuneString to dispose.
void rune_string_destroy(RuneString* storage);

/// Copies a RuneString from source to destination.
/// @param destination Pointer to the destination RuneString.
/// @param source Pointer to the source RuneString.
void rune_string_copy(RuneString* destination, RuneString const* source);
/// @}

/// @name Mutation
/// @{

/// Assigns a raw byte buffer to an existing RuneString, reallocating if necessary.
/// @param string Pointer to the RuneString.
/// @param bytes Pointer to the byte buffer.
/// @param length Length of the byte buffer in bytes.
/// @return True on success, false on allocation failure.
bool rune_string_assign_bytes(RuneString* string, char const* bytes, usize length);
/// @}

/// @name Conversion Constructors
/// @{

/// Converts an 8-bit signed integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 8-bit signed integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_int8(RuneAllocator const* allocator, int8 value);

/// Converts a 16-bit signed integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 16-bit signed integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_int16(RuneAllocator const* allocator, int16 value);

/// Converts a 32-bit signed integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 32-bit signed integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_int32(RuneAllocator const* allocator, int32 value);

/// Converts a 64-bit signed integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 64-bit signed integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_int64(RuneAllocator const* allocator, int64 value);

/// Converts an 8-bit unsigned integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 8-bit unsigned integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_uint8(RuneAllocator const* allocator, uint8 value);

/// Converts a 16-bit unsigned integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 16-bit unsigned integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_uint16(RuneAllocator const* allocator, uint16 value);

/// Converts a 32-bit unsigned integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 32-bit unsigned integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_uint32(RuneAllocator const* allocator, uint32 value);

/// Converts a 64-bit unsigned integer to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 64-bit unsigned integer to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_uint64(RuneAllocator const* allocator, uint64 value);

/// Converts a 32-bit floating-point value to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 32-bit float value to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_float32(RuneAllocator const* allocator, float32 value);

/// Converts a 64-bit floating-point value to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The 64-bit float value to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_float64(RuneAllocator const* allocator, float64 value);

/// Converts a boolean value to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The boolean value to convert.
/// @return The constructed RuneString.
RuneString rune_string_from_bool(RuneAllocator const* allocator, bool value);

/// Converts a RuneStringSlice to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The RuneStringSlice to copy.
/// @return The constructed RuneString.
RuneString rune_string_from_stringslice(RuneAllocator const* allocator, RuneStringSlice const* value);

/// Converts a C-style string (RuneCString) to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The C-style string to copy.
/// @return The constructed RuneString.
RuneString rune_string_from_cstring(RuneAllocator const* allocator, RuneCString const* value);

/// Converts a borrowed C string slice to a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param value The borrowed C string slice to copy.
/// @return The constructed RuneString.
RuneString rune_string_from_cstring_slice(RuneAllocator const* allocator, RuneCStringSlice const* value);
/// @}

/// @name Queries
/// @{

/// Computes the number of UTF-8 code points in the string.
/// @param self Pointer to the RuneString.
/// @return The number of code points.
intptr rune_string_count(RuneString const* self);

/// Returns the byte length of the string.
/// @param self Pointer to the RuneString.
/// @return The byte length of the string.
usize rune_string_byte_length(RuneString const* self);

/// Checks if the string is empty.
/// @param self Pointer to the RuneString.
/// @return True if empty, false otherwise.
bool rune_string_is_empty(RuneString const* self);

/// Accesses the raw UTF-8 byte array data of the string.
/// @param self Pointer to the RuneString.
/// @return The underlying byte buffer, or null.
char const* rune_string_data(RuneString const* self);

/// Decodes and returns the character (code point) at the given scalar index.
/// @param self Pointer to the RuneString.
/// @param at The scalar character index.
/// @return The Unicode code point, or 0 if index is out of bounds.
RuneChar rune_string_char_at(RuneString const* self, intptr at);

/// Creates an iterator over the string's Unicode scalar values.
/// @param self String to iterate.
/// @return Initialised iterator.
RuneStringIterator rune_string_iterator_create(RuneString const* self);

/// Advances a string iterator.
/// @param self Iterator to advance.
/// @param out_value Receives the next character on success.
/// @return True when a character was produced.
bool rune_string_iterator_next(RuneStringIterator* self, RuneChar* out_value);

/// Computes the hash value of the string.
/// @param self Pointer to the RuneString.
/// @return The hash code.
int64 rune_string_hash(RuneString const* self);

/// Checks if the string contains a given substring.
/// @param self Pointer to the RuneString.
/// @param substring Pointer to the substring.
/// @return True if contains, false otherwise.
bool rune_string_contains(RuneString const* self, RuneString const* substring);

/// Compares two RuneStrings for equality.
/// @param left Pointer to the left string.
/// @param right Pointer to the right string.
/// @return True if they are identical, false otherwise.
bool rune_string_equal(RuneString const* left, RuneString const* right);

/// Compares two RuneStrings lexicographically.
/// @param left Pointer to the left string.
/// @param right Pointer to the right string.
/// @return Neg/0/Pos value indicating ordering.
int8 rune_string_compare(RuneString const* left, RuneString const* right);
/// @}

/// @name Derived Values
/// @{

/// Concatenates two RuneStrings and returns the result.
/// @param allocator Allocator used for heap storage.
/// @param left Pointer to the left string.
/// @param right Pointer to the right string.
/// @return The concatenated RuneString.
RuneString rune_string_add(RuneAllocator const* allocator, RuneString const* left, RuneString const* right);

/// Concatenates a RuneString and a RuneStringSlice, returning the result.
/// @param allocator Allocator used for heap storage.
/// @param left Pointer to the left string.
/// @param right Pointer to the right string slice.
/// @return The concatenated RuneString.
RuneString rune_string_add_slice(RuneAllocator const* allocator, RuneString const* left, RuneStringSlice const* right);

/// Converts a RuneString to lowercase and returns the result.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @return The lowercase RuneString.
RuneString rune_string_lower(RuneAllocator const* allocator, RuneString const* self);

/// Converts a RuneString to uppercase and returns the result.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @return The uppercase RuneString.
RuneString rune_string_upper(RuneAllocator const* allocator, RuneString const* self);

/// Extracts a substring of a RuneString.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @param start Starting index of the substring.
/// @param end Ending index of the substring.
/// @return The extracted substring as a RuneString.
RuneString rune_string_substring(RuneAllocator const* allocator, RuneString const* self, intptr start, intptr end);

/// Inserts a character into a RuneString at a given index.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @param character The character to insert.
/// @param at Index at which to insert the character.
/// @return The modified RuneString.
RuneString rune_string_insert_character_at(
    RuneAllocator const* allocator,
    RuneString const* self,
    RuneChar character,
    intptr at);

/// Removes a character from a RuneString at a given index.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @param at Index of the character to remove.
/// @return The modified RuneString.
RuneString rune_string_remove_character_at(RuneAllocator const* allocator, RuneString const* self, intptr at);

/// Replaces a character in a RuneString at a given index.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the source string.
/// @param at Index of the character to replace.
/// @param with The new character to insert at the index.
/// @return The modified RuneString.
RuneString rune_string_replace_character_at(
    RuneAllocator const* allocator,
    RuneString const* self,
    intptr at,
    RuneChar with);

/// Splits a RuneString by a given separator string, returning a RuneArray of substrings.
/// @param allocator Allocator used for heap storage.
/// @param self Pointer to the string to split.
/// @param separator Pointer to the separator string.
/// @return A RuneArray containing the split substrings.
RuneArray rune_string_split(RuneAllocator const* allocator, RuneString const* self, RuneString const* separator);
/// @}

#ifdef __cplusplus
}
#endif
