#ifndef __VALIDATOR_TOKENIZER_H__
#define __VALIDATOR_TOKENIZER_H__

#include <stdint.h>

typedef enum
{
    VTT_NEWLINE,
    VTT_INTEGER,
    VTT_FLOAT,
    VTT_STRING,
    VTT_EOF
} ValidatorTokenizerTokenTypeT;

typedef struct
{
    ValidatorTokenizerTokenTypeT type;
    char* text;
    uint32_t length;
    int32_t line;
    int32_t pos;
} ValidatorTokenizerTokenT;

// Allocates memory for the next token and returns pointer to it.
// Token structure and its "text" field should be freed by user.
ValidatorTokenizerTokenT *ValidatorTokenizerNextToken();

// Set data source for tokenizer (default is stdin).
void ValidatorTokenizerSetInput(FILE *handle);

#endif // __VALIDATOR_TOKENIZER_H__
