#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <inttypes.h>

enum tokenType
{
    ttNewLine,
    ttInteger,
    ttFloat,
    ttString,
    ttEof,
};

int32_t yylex();
int32_t yycurline();
int32_t yycurpos();
size_t yytoklen();
char* yytoktext();
char* yytoktextcopy();

#endif // __TOKENIZER_H__
