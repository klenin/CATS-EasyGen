#ifndef __PARSER_OBJECT_ATTRIBUTES_H__
#define __PARSER_OBJECT_ATTRIBUTES_H__

typedef enum
{
    PARSER_OBJECT_KIND_NEWLINE,
    PARSER_OBJECT_KIND_SOFTLINE,
    PARSER_OBJECT_KIND_INTEGER,
    PARSER_OBJECT_KIND_FLOAT,
    PARSER_OBJECT_KIND_STRING,
    PARSER_OBJECT_KIND_SEQUENCE,
    PARSER_OBJECT_KIND_END,

    PARSER_OBJECT_KINDS_COUNT
} ParserObjectKindT;

typedef enum
{
    PARSER_OBJECT_ATTR_NAME,
    PARSER_OBJECT_ATTR_RANGE,
    PARSER_OBJECT_ATTR_DIGITS,
    PARSER_OBJECT_ATTR_CHARS,
    PARSER_OBJECT_ATTR_LENGTH,
    PARSER_OBJECT_ATTR_LENGTH_RANGE,

    PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT
} ParserObjectAttributeKindT;

typedef enum
{
    PARSER_OBJECT_ATTR_VALUE_IDENTIFIER,
    PARSER_OBJECT_ATTR_VALUE_INTEGER,
    PARSER_OBJECT_ATTR_VALUE_INTEGER_RANGE,
    PARSER_OBJECT_ATTR_VALUE_CHARSET,

    PARSER_OBJECT_ATTRIBUTE_VALUES_COUNT
} ParserObjectAttributeValueKindT;

extern const char* g_ParserObjectKind2Str[PARSER_OBJECT_KINDS_COUNT];
extern const char*
    g_ParserObjectAttrKind2Str[PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT];
extern const ParserObjectAttributeValueKindT
    g_ParserObjectKind2AttrValueKind[PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT];
extern const int
    g_ParserObjectAttrKindIsValidForObjectKind[PARSER_OBJECT_KINDS_COUNT]
                                          [PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT];

#endif // __PARSER_OBJECT_ATTRIBUTES_H__
