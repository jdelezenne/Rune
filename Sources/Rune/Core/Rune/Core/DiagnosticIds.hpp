#pragma once

#include "Rune/Core/StringView.hpp"

namespace rune
{
    /// Stable diagnostic identifiers.
    namespace DiagnosticIds
    {
#pragma region Lexer

        inline constexpr StringView lexerFileReadFailed = "lexer.file.read_failed";
        inline constexpr StringView lexerSourceInvalidUtf8 = "lexer.source.invalid_utf8";
        inline constexpr StringView lexerUnexpectedCharacter = "lexer.unexpected_character";
        inline constexpr StringView lexerUnexpectedUnicodeCharacter = "lexer.unexpected_unicode_character";
        inline constexpr StringView lexerStringNewlineInSingleLine = "lexer.string.newline_in_single_line";
        inline constexpr StringView lexerStringUnterminated = "lexer.string.unterminated";
        inline constexpr StringView lexerStringMultilineUnterminated = "lexer.string.multiline_unterminated";
        inline constexpr StringView lexerStringInterpolationUnterminated = "lexer.string.interpolation_unterminated";
        inline constexpr StringView lexerStringInterpolationNewline = "lexer.string.interpolation_newline";
        inline constexpr StringView lexerEscapeInvalid = "lexer.escape.invalid";
        inline constexpr StringView lexerEscapeNonAscii = "lexer.escape.non_ascii";
        inline constexpr StringView lexerUnicodeEscapeInvalidFormat = "lexer.unicode_escape.invalid_format";
        inline constexpr StringView lexerUnicodeEscapeInvalidCharacter = "lexer.unicode_escape.invalid_character";
        inline constexpr StringView lexerUnicodeEscapeTooLong = "lexer.unicode_escape.too_long";
        inline constexpr StringView lexerUnicodeEscapeUnterminated = "lexer.unicode_escape.unterminated";
        inline constexpr StringView lexerUnicodeEscapeEmpty = "lexer.unicode_escape.empty";
        inline constexpr StringView lexerUnicodeEscapeInvalidHex = "lexer.unicode_escape.invalid_hex";
        inline constexpr StringView lexerUnicodeEscapeOutOfRange = "lexer.unicode_escape.out_of_range";
        inline constexpr StringView lexerUnicodeEscapeSurrogate = "lexer.unicode_escape.surrogate";
        inline constexpr StringView lexerCharEmpty = "lexer.char.empty";
        inline constexpr StringView lexerCharUnterminated = "lexer.char.unterminated";
        inline constexpr StringView lexerCharNewline = "lexer.char.newline";
        inline constexpr StringView lexerRawStringUnterminated = "lexer.raw_string.unterminated";
        inline constexpr StringView lexerRawStringUnexpectedHash = "lexer.raw_string.unexpected_hash";
        inline constexpr StringView lexerNumberHexEmpty = "lexer.number.hex_empty";
        inline constexpr StringView lexerNumberHexInvalid = "lexer.number.hex_invalid";
        inline constexpr StringView lexerNumberBinaryEmpty = "lexer.number.binary_empty";
        inline constexpr StringView lexerNumberBinaryInvalid = "lexer.number.binary_invalid";
        inline constexpr StringView lexerNumberOctalEmpty = "lexer.number.octal_empty";
        inline constexpr StringView lexerNumberOctalInvalid = "lexer.number.octal_invalid";
        inline constexpr StringView lexerNumberHexFloatExponent = "lexer.number.hex_float_exponent";
        inline constexpr StringView lexerNumberHexFloatExponentDigits = "lexer.number.hex_float_exponent_digits";
        inline constexpr StringView lexerNumberHexFloatInvalidExponent = "lexer.number.hex_float_invalid_exponent";
        inline constexpr StringView lexerNumberScientificDigits = "lexer.number.scientific_digits";
        inline constexpr StringView lexerNumberFloatInvalid = "lexer.number.float_invalid";
        inline constexpr StringView lexerNumberIntegerInvalid = "lexer.number.integer_invalid";
        inline constexpr StringView lexerCommentUnterminated = "lexer.comment.unterminated";

#pragma endregion

#pragma region Parser

        inline constexpr StringView parserExpectedToken = "parser.expected_token";
        inline constexpr StringView parserSyntax = "parser.syntax";
        inline constexpr StringView parserInterfaceRequirementBody = "parser.interface.requirement_body";
        inline constexpr StringView parserInterfaceAccessorBody = "parser.interface.accessor_body";
        inline constexpr StringView parserInterfaceAccessModifier = "parser.interface.access_modifier";
        inline constexpr StringView parserAttributeDuplicate = "parser.attribute.duplicate";
        inline constexpr StringView parserAttributeInvalidOnGroup = "parser.attribute.invalid_on_group";
        inline constexpr StringView parserAttributeInvalidTarget = "parser.attribute.invalid_target";
        inline constexpr StringView parserModifierFinalOnStruct = "parser.modifier.final_on_struct";
        inline constexpr StringView parserModifierOpenWithoutStruct = "parser.modifier.open_without_struct";
        inline constexpr StringView parserMainHasParameters = "parser.main.has_parameters";
        inline constexpr StringView parserMainInvalidReturnType = "parser.main.invalid_return_type";
        inline constexpr StringView parserMainDuplicate = "parser.main.duplicate";
        inline constexpr StringView parserInterpolationTokenizeFailed = "parser.interpolation.tokenize_failed";
        inline constexpr StringView parserInterpolationInvalid = "parser.interpolation.invalid";
        inline constexpr StringView parserInterpolationParseFailed = "parser.interpolation.parse_failed";

#pragma endregion

#pragma region Sema: module / driver

        inline constexpr StringView semaModuleNameRequired = "sema.module.name_required";
        inline constexpr StringView semaModuleNotFound = "sema.module.not_found";
        inline constexpr StringView semaModuleLoadFailed = "sema.module.load_failed";
        inline constexpr StringView semaModuleDuplicateImport = "sema.module.duplicate_import";

#pragma endregion

#pragma region Sema: attributes

        inline constexpr StringView semaAttributeUnknown = "sema.attribute.unknown";
        inline constexpr StringView semaAttributeInvalidTarget = "sema.attribute.invalid_target";
        inline constexpr StringView semaAttributeArgument = "sema.attribute.argument";
        inline constexpr StringView semaAttributeDuplicate = "sema.attribute.duplicate";
        inline constexpr StringView semaLayoutInvalidUse = "sema.layout.invalid_use";
        inline constexpr StringView semaFfiCString = "sema.ffi.cstring";
        inline constexpr StringView semaFfiStructLayout = "sema.ffi.struct_layout";
        inline constexpr StringView semaFfiRuntimeType = "sema.ffi.runtime_type";
        inline constexpr StringView semaIntrinsicRestricted = "sema.intrinsic.restricted";

#pragma endregion

#pragma region Sema: lookup

        inline constexpr StringView semaLookupNotFound = "sema.lookup.not_found";
        inline constexpr StringView semaLookupUndefinedVariable = "sema.lookup.undefined_variable";
        inline constexpr StringView semaLookupUndefinedIdentifier = "sema.lookup.undefined_identifier";
        inline constexpr StringView semaLookupNestedFunctionCapture = "sema.lookup.nested_function_capture";
        inline constexpr StringView semaLookupDiscardWildcard = "sema.lookup.discard_wildcard";
        inline constexpr StringView semaLookupMemberNotFound = "sema.lookup.member_not_found";
        inline constexpr StringView semaLookupModuleMemberNotFound = "sema.lookup.module_member_not_found";
        inline constexpr StringView semaAccessInaccessible = "sema.access.inaccessible";

#pragma endregion

#pragma region Sema: types

        inline constexpr StringView semaTypeMismatch = "sema.type.mismatch";
        inline constexpr StringView semaTypeSelfInvalid = "sema.type.self_invalid";
        inline constexpr StringView semaUnhandledAst = "sema.unhandled.ast";

#pragma endregion

#pragma region Sema: declarations

        inline constexpr StringView semaRedefinition = "sema.redefinition";
        inline constexpr StringView semaAliasCompilerKnownName = "sema.alias.compiler_known_name";

#pragma endregion

#pragma region Sema: assignments / variables

        inline constexpr StringView semaAssignImmutable = "sema.assign.immutable";
        inline constexpr StringView semaAssignSelfNotMutating = "sema.assign.self_not_mutating";
        inline constexpr StringView semaInitRequired = "sema.init.required";
        inline constexpr StringView semaAccessModifierOnLocalVariable = "sema.access.modifier_on_local_variable";
        inline constexpr StringView semaUseBeforeInit = "sema.use.before_create";
        inline constexpr StringView semaVarNeverAssigned = "sema.var.never_assigned";

#pragma endregion

#pragma region Sema: literals / collections

        inline constexpr StringView semaLiteralOutOfRange = "sema.literal.out_of_range";
        inline constexpr StringView semaLiteralNoContext = "sema.literal.no_context";
        inline constexpr StringView semaLiteralUnsupportedToken = "sema.literal.unsupported_token";
        inline constexpr StringView semaLiteralNegationOperand = "sema.literal.negation_operand";
        inline constexpr StringView semaLiteralOverflow = "sema.literal.overflow";
        inline constexpr StringView semaLiteralStringMismatch = "sema.literal.string_mismatch";
        inline constexpr StringView semaLiteralNilNotOption = "sema.literal.nil_not_option";
        inline constexpr StringView semaLiteralCollectionElementMismatch = "sema.literal.collection_element_mismatch";
        inline constexpr StringView semaLiteralCollectionContextRequired = "sema.literal.collection_context_required";

#pragma endregion

#pragma region Sema: calls / structs

        inline constexpr StringView semaCallArgumentCount = "sema.call.argument_count";
        inline constexpr StringView semaStructMissingRequiredField = "sema.struct.missing_required_field";
        inline constexpr StringView semaStructUnknownInitializerArg = "sema.struct.unknown_initializer_arg";
        inline constexpr StringView semaStructConstInitializerArg = "sema.struct.const_initializer_arg";

#pragma endregion

#pragma region Sema: tuples

        inline constexpr StringView semaTupleDestructuringRequiresInit = "sema.tuple.destructuring_requires_create";
        inline constexpr StringView semaTuplePatternMismatch = "sema.tuple.pattern_mismatch";
        inline constexpr StringView semaTuplePatternSizeMismatch = "sema.tuple.pattern_size_mismatch";
        inline constexpr StringView semaTupleElementIndexInvalid = "sema.tuple.element_index_invalid";
        inline constexpr StringView semaTupleElementIndexOutOfRange = "sema.tuple.element_index_out_of_range";
        inline constexpr StringView semaTupleElementLabelRequired = "sema.tuple.element_label_required";
        inline constexpr StringView semaTupleElementLabelUnknown = "sema.tuple.element_label_unknown";
        inline constexpr StringView semaTupleElementLabelMismatch = "sema.tuple.element_label_mismatch";

#pragma endregion

#pragma region Sema: functions / control flow

        inline constexpr StringView semaFunctionReturnVoidWithValue = "sema.function.return_void_with_value";
        inline constexpr StringView semaFunctionReturnMissingValue = "sema.function.return_missing_value";
        inline constexpr StringView semaThrowsOutsideThrowingFunction = "sema.throws.outside_throwing_function";
        inline constexpr StringView semaEntryPointTopLevelExecutable = "sema.entry_point.top_level_executable";

#pragma endregion

#pragma region Sema: defer

        inline constexpr StringView semaDeferModuleLevel = "sema.defer.module_level";
        inline constexpr StringView semaDeferInitFunction = "sema.defer.init_function";
        inline constexpr StringView semaDeferFinalFunction = "sema.defer.final_function";

#pragma endregion

#pragma region Sema: Option / Result

        inline constexpr StringView semaOptionForceUnwrap = "sema.option.force_unwrap";
        inline constexpr StringView semaResultTryQuestion = "sema.result.try_question";
        inline constexpr StringView semaResultTryPropagating = "sema.result.try_propagating";
        inline constexpr StringView semaResultTryErrorMismatch = "sema.result.try_error_mismatch";
        inline constexpr StringView semaAsyncAwaitOutsideAsync = "sema.async.await_outside_async";
        inline constexpr StringView semaAsyncCallWithoutAwait = "sema.async.call_without_await";
        inline constexpr StringView semaAsyncCallFromSync = "sema.async.call_from_sync";
        inline constexpr StringView semaAsyncAwaitNonAsyncCall = "sema.async.await_non_async_call";

#pragma endregion

#pragma region Sema: references

        inline constexpr StringView semaRefDerefNonReference = "sema.ref.deref_non_reference";
        inline constexpr StringView semaRefImmutableDerefAssign = "sema.ref.immutable_deref_assign";
        inline constexpr StringView semaRefAddressOfNonLvalue = "sema.ref.address_of_non_lvalue";
        inline constexpr StringView semaRefImmutableAddressOf = "sema.ref.immutable_address_of";
        inline constexpr StringView semaRefImmutableMemberAssign = "sema.ref.immutable_member_assign";
        inline constexpr StringView semaRefImmutableMutatingMethod = "sema.ref.immutable_mutating_method";
        inline constexpr StringView semaRefBorrowConflict = "sema.ref.borrow_conflict";
        inline constexpr StringView semaRefMutableBorrowImmutable = "sema.ref.mutable_borrow_immutable";

#pragma endregion

#pragma region Sema: enum / flags / union

        inline constexpr StringView semaEnumMissingRawType = "sema.enum.missing_raw_type";
        inline constexpr StringView semaEnumMixedValues = "sema.enum.mixed_values";
        inline constexpr StringView semaEnumImplicitValuesRequireInteger = "sema.enum.implicit_values_require_integer";
        inline constexpr StringView semaEnumRawTypeMustBeInteger = "sema.enum.raw_type_must_be_integer";
        inline constexpr StringView semaFlagsMissingRawType = "sema.flags.missing_raw_type";
        inline constexpr StringView semaFlagsRawTypeMustBeInteger = "sema.flags.raw_type_must_be_integer";

#pragma endregion

#pragma region Sema: match

        inline constexpr StringView semaMatchNonMatchable = "sema.match.non_matchable";
        inline constexpr StringView semaMatchUnknownCase = "sema.match.unknown_case";
        inline constexpr StringView semaMatchDuplicateCase = "sema.match.duplicate_case";
        inline constexpr StringView semaMatchBindingArity = "sema.match.binding_arity";
        inline constexpr StringView semaMatchUnitCaseBindings = "sema.match.unit_case_bindings";
        inline constexpr StringView semaMatchNonExhaustive = "sema.match.non_exhaustive";
        inline constexpr StringView semaMatchMultipleDefault = "sema.match.multiple_default";

#pragma endregion

#pragma region Sema: cast

        inline constexpr StringView semaCastIncompatible = "sema.cast.incompatible";
        inline constexpr StringView semaCastUnrelated = "sema.cast.unrelated";
        inline constexpr StringView semaCastSlices = "sema.cast.slices";
        inline constexpr StringView semaCastUseConditional = "sema.cast.use_conditional";

#pragma endregion

#pragma region Sema: memory / make / dispose

        inline constexpr StringView semaMakeAllocatorNotStable = "sema.make.allocator_not_stable";
        inline constexpr StringView semaMakeAllocatorNotConforming = "sema.make.allocator_not_conforming";
        inline constexpr StringView semaMakeExplicitAllocatorRequiresForced =
            "sema.make.requires_forced_in_non_throwing";
        inline constexpr StringView semaDisposeNotReference = "sema.dispose.not_reference";
        inline constexpr StringView semaDisposeAllocatorNotStable = "sema.dispose.allocator_not_stable";
        inline constexpr StringView semaDisposeAllocatorNotConforming = "sema.dispose.allocator_not_conforming";

#pragma endregion

#pragma region Sema: index / for

        inline constexpr StringView semaIndexNonCollection = "sema.index.non_collection";
        inline constexpr StringView semaIndexImmutableSlice = "sema.index.immutable_slice";
        inline constexpr StringView semaForNonIterable = "sema.for.non_iterable";
        inline constexpr StringView semaForStrideNotRange = "sema.for.stride_not_range";
        inline constexpr StringView semaForStrideZero = "sema.for.stride_zero";
        inline constexpr StringView semaForStrideNotPositive = "sema.for.stride_not_positive";

#pragma endregion

#pragma region Sema: interface

        inline constexpr StringView semaInterfaceBareValueType = "sema.interface.bare_value_type";
        inline constexpr StringView semaInterfaceInheritanceSelf = "sema.interface.inheritance.self";
        inline constexpr StringView semaInterfaceInheritanceCyclic = "sema.interface.inheritance.cyclic";
        inline constexpr StringView semaInterfaceInheritanceDuplicate = "sema.interface.inheritance.duplicate";
        inline constexpr StringView semaInterfaceExtendNotInterface = "sema.interface.extend.not_interface";
        inline constexpr StringView semaInterfaceIncompatibleSignature = "sema.interface.incompatible_signature";
        inline constexpr StringView semaInterfaceDuplicateRequirement = "sema.interface.duplicate_requirement";
        inline constexpr StringView semaInterfaceMissingMember = "sema.interface.missing_member";
        inline constexpr StringView semaInterfaceWitnessNotPublic = "sema.interface.witness.not_public";
        inline constexpr StringView semaInterfaceWitnessThrowsMismatch = "sema.interface.witness.throws_mismatch";
        inline constexpr StringView semaInterfaceWitnessNotSettable = "sema.interface.witness.not_settable";
        inline constexpr StringView semaBuiltinInstanceField = "sema.builtin.instance_field";

#pragma endregion

#pragma region Sema: foreign

        inline constexpr StringView semaForeignBinding = "sema.foreign.binding";

#pragma endregion
    }
}
