#ifndef __PARSER_ERROR_H__
#define __PARSER_ERROR_H__

// E_ means error, B_ means bug.
enum parseErrorCode
{
    E_SUCCESS = 0,

    // Start of range.
    E_ERROR_RANGE_START = 1,

    // Tokenizer errors.
    E_UNKNOWN_TOKEN = 1,
    E_UNEXPECTED_TOKEN,
    B_UNKNOWN_STATE,
    E_TOO_LONG_INTEGER,
    E_LCB_EXPECTED,
    E_RCB_EXPECTED,
    E_RB_EXPECTED,
    E_UNEXPECTED_CHAR_AFTER_BACKSLASH,

    // Parser errors.
    E_OPERAND_EXPECTED,
    B_UNEXPECTED_CAP,
    B_UNKNOWN_OPERATION,
    B_UNKNOWN_IDENTIFIER,
    E_UNKNOWN_OBJECT,
    E_UNINITIALIZED_OBJECT,
    E_DUPLICATE_OBJECT,
    E_INTEGER_OBJECT_EXPECTED,
    E_OBJECT_KIND_MISMATCH_GET,
    E_OBJECT_KIND_EXPECTED,
    E_UNKNOWN_ATTRIBUTE,
    E_INVALID_CHARSET, // need further investigation
    E_ATTRIBUTE_ALREADY_DEFINED,
    E_INVALID_ATTRIBUTE,
    E_INVALID_ATTRIBUTE_LIST,
    B_ARRAY_ATTR_TYPE_BY_KIND,
    E_UNEXPECTED_END,
    E_END_EXPECTED,

    // Generator errors.
    E_ASSIGN_NON_INT,
    E_ASSIGN_NON_FLOAT,
    E_ASSIGN_NON_STRING,
    E_NOT_A_SEQUENCE,
    E_OUT_OF_RANGE,
    E_VALUE_ALREADY_DEFINED,
    E_INVALID_FRACTIONAL_PART,
    E_INVALID_STRING_LENGTH,
    E_INVALID_CHARS,
    E_GENERATE_NON_INT,
    E_GENERATE_NON_FLOAT,
    E_GENERATE_NON_STRING,
    E_GENERATE_NON_SEQUENCE,
    E_DESTROY_NON_SEQUENCE,
    E_INVALID_INDEX,
    E_ZERO_DIVISION,

    E_EXCEPTION,

    E_UNKNOWN_ERROR,

    // End of range.
    E_NUMBER_OF_ERROR_TYPES,
};

struct _errordesc {
    int  code;
    char *message;
} errorMessages[] = {
    { E_SUCCESS, "no error" },

    { E_UNKNOWN_TOKEN, "unknown token" },
    { E_UNEXPECTED_TOKEN, "unexpected token" },
    { B_UNKNOWN_STATE, "bug: unknown state" },
    { E_TOO_LONG_INTEGER, "too long integer constant" },
    { E_LCB_EXPECTED, "'{' expected" },
    { E_RCB_EXPECTED, "'}' expected" },
    { E_RB_EXPECTED, "')' expected" },
    { E_UNEXPECTED_CHAR_AFTER_BACKSLASH, "unexpected char after '\\'" },

    { E_OPERAND_EXPECTED, "operand expected" },
    { B_UNEXPECTED_CAP, "bug: unexpected '^'" },
    { B_UNKNOWN_OPERATION, "bug: unknown arithmetic operation" },
    { B_UNKNOWN_IDENTIFIER, "bug: unknown identifier" },
    { E_UNKNOWN_OBJECT, "unknown object name" },
    { E_UNINITIALIZED_OBJECT, "getting value from uninitialized object" },
    { E_DUPLICATE_OBJECT, "duplicate object name" },
    { E_INTEGER_OBJECT_EXPECTED, "integer object expected" },
    { E_OBJECT_KIND_MISMATCH_GET, "object kind mismatch while getting value" },
    { E_OBJECT_KIND_EXPECTED, "object kind expected" },
    { E_UNKNOWN_ATTRIBUTE, "unknown attribute name" },
    { E_INVALID_CHARSET, "invalid charset" },
    { E_ATTRIBUTE_ALREADY_DEFINED, "attribute is already defined" },
    { E_INVALID_ATTRIBUTE, "attribute is invalid for this type of object" },
    { E_INVALID_ATTRIBUTE_LIST, "invalid attribute list" },
    { B_ARRAY_ATTR_TYPE_BY_KIND, "bug: error in attrTypeByKind[]" },
    { E_UNEXPECTED_END, "unexpected 'end'" },
    { E_END_EXPECTED, "'end' expected" },

    { E_ASSIGN_NON_INT, "attempt to assign a non-integer value to integer object" },
    { E_ASSIGN_NON_FLOAT, "attempt to assign a non-float value to float object" },
    { E_ASSIGN_NON_STRING, "attempt to assign a non-string value to string object" },
    { E_NOT_A_SEQUENCE, "attempt to get length of a non-sequence object" },
    { E_OUT_OF_RANGE, "number is out of range" },
    { E_VALUE_ALREADY_DEFINED, "attempt to set already defined value" },
    { E_INVALID_FRACTIONAL_PART, "invalid number of digits after decimal point" },
    { E_INVALID_STRING_LENGTH, "invalid string length" },
    { E_INVALID_CHARS, "attempt to assign invalid characters to string object" },
    { E_GENERATE_NON_INT, "attempt to generate integer value for non-integer object" },
    { E_GENERATE_NON_FLOAT, "attempt to generate float value for non-float object" },
    { E_GENERATE_NON_STRING, "attempt to generate string value for non-string object" },
    { E_GENERATE_NON_SEQUENCE, "attempt to generate sequence value for non-sequence object" },
    { E_DESTROY_NON_SEQUENCE, "attempt to destroy non-sequence object as sequence" },
    { E_INVALID_INDEX, "sequence index is out of range" },
    { E_ZERO_DIVISION, "attempt to divide by zero" },

    { E_EXCEPTION, "exception" },

    { E_UNKNOWN_ERROR, "unknown error" },
};

#endif // __PARSER_ERROR_H__
