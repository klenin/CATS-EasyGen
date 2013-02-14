#include "ParserObjectAttributes.h"

const char* g_ParserObjectKind2Str[PARSER_OBJECT_KINDS_COUNT] =
{
    "newline",
    "softline",
    "integer",
    "float",
    "string",
    "seq",
    "end"
};

const char* g_ParserObjectAttrKind2Str[PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT] =
{
    "name",
    "range",
    "digits",
    "chars",
    "length",
    "lenrange"
};

const ParserObjectAttributeValueKindT
    g_ParserObjectKind2AttrValueKind[PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT] =
{
    PARSER_OBJECT_ATTR_VALUE_IDENTIFIER,
    PARSER_OBJECT_ATTR_VALUE_INTEGER_RANGE,
    PARSER_OBJECT_ATTR_VALUE_INTEGER,
    PARSER_OBJECT_ATTR_VALUE_CHARSET,
    PARSER_OBJECT_ATTR_VALUE_INTEGER,
    PARSER_OBJECT_ATTR_VALUE_INTEGER_RANGE
};

const int
g_ParserObjectAttrKindIsValidForObjectKind[PARSER_OBJECT_KINDS_COUNT]
                                          [PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT]
                                           =
{
    {0, 0, 0, 0, 0, 0}, // Newline
    {0, 0, 0, 0, 0, 0}, // Softline
    {1, 1, 0, 0, 0, 0}, // Integer: name, range
    {1, 1, 1, 0, 0, 0}, // Float: name, range, digits
    {1, 0, 0, 1, 0, 1}, // String: name, chars, length range
    {1, 0, 0, 0, 1, 0}, // Sequence: name, length
    {0, 0, 0, 0, 0, 0}  // End
};
