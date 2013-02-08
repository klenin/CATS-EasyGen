#ifndef __VALIDATOR_TOKENIZER_H__
#define __VALIDATOR_TOKENIZER_H__

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
    int32_t line;
    int32_t pos;
} Token;

// Allocates memory for the next token and returns pointer to it.
// Token structure and its "text" field should be freed by user.
Token *yynexttoken();

#endif // __VALIDATOR_TOKENIZER_H__
