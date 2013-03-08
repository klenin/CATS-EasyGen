#ifndef __VALIDATOR_TOKENIZER_H__
#define __VALIDATOR_TOKENIZER_H__

#include <stdint.h>

typedef enum
{
    VTT_NEWLINE,
    VTT_INTEGER,
    VTT_FLOAT,
    VTT_STRING,
    VTT_EOF,
    VTT_COUNT
} ValidatorTokenizerTokenTypeT;

typedef struct
{
    ValidatorTokenizerTokenTypeT type;
    char* text;
    uint32_t length;
    int32_t line;
    int32_t pos;
} ValidatorTokenizerTokenT;

extern const char* g_ValidatorTokenizerTokenType2Str[VTT_COUNT];

// Allocates memory for the next token and returns pointer to it.
ValidatorTokenizerTokenT *ValidatorTokenizerNextToken();

// Destroys token structure.
void ValidatorTokenizerDestroyToken(ValidatorTokenizerTokenT *token);

// Set data source for tokenizer (default is stdin).
void ValidatorTokenizerSetInput(FILE *handle);

int32_t ValidatorTokenizerGetCurrentLine();
int32_t ValidatorTokenizerGetCurrentPosition();

#endif // __VALIDATOR_TOKENIZER_H__
