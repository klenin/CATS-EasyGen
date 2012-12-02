#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdint.h>

typedef enum
{
    ttNewLine,
    ttInteger,
    ttFloat,
    ttString,
    ttEof,
} TokenType;

typedef struct
{
    TokenType type;
    char* text;
    uint32_t length;
} Token;

int32_t yylex();
int32_t yycurline();
int32_t yycurpos();
uint32_t yytoklen();
char* yytoktext();
char* yytoktextcopy();

#endif // __TOKENIZER_H__
