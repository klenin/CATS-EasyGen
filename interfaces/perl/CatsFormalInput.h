#ifndef __CATS_FORMAL_INPUT_H__
#define __CATS_FORMAL_INPUT_H__

#include <stdlib.h>

typedef struct
{
    int line, pos;
    int code;
} ParserErrorT;

typedef struct
{
    char *message;
    int line, pos;
} ValidatorErrorT;

ParserErrorT *ParserValidateFormatDescription(const char *formatDescription);
const char *ParserGetErrorMessage(ParserErrorT *error);
int ParserGetErrorLine(ParserErrorT *error);
int ParserGetErrorPos(ParserErrorT *error);

ValidatorErrorT *ValidatorValidate(char *inputFilename, char *formatDescription);
const char *ValidatorErrorGetErrorMessage(ValidatorErrorT *error);
int ValidatorErrorGetErrorLine(ValidatorErrorT *error);
int ValidatorErrorGetErrorPos(ValidatorErrorT *error);

#endif // __CATS_FORMAL_INPUT_H__
