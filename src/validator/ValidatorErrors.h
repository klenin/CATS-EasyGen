#ifndef __VALIDATOR_ERRORS_H__
#define __VALIDATOR_ERRORS_H__

#include <stdint.h>

typedef struct
{
    char *message;
    int line;
    int pos;
} ValidatorErrorT;

ValidatorErrorT *ValidatorGetLastError();
int32_t ValidatorIsErrorRaised();
void ValidatorRaiseError(const char *message);
void ValidatorRaiseErrorEx(const char *message, int32_t line, int32_t pos);

#endif // __VALIDATOR_ERRORS_H__
