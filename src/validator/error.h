#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdint.h>

typedef struct
{
    int32_t line;
    int32_t pos;
    int32_t code;
} ValidatorErrorT;

typedef enum
{
    E_SUCCESS = 0,

    // Start of range.
    E_ERROR_RANGE_START = 1,

    E_CANNOT_READ_INPUT_FILE,
    E_CANNOT_READ_FORMAT_DESCRIPTION_FILE,
    E_NOT_ENOUGH_MEMORY,

    // End of range.
    E_NUMBER_OF_ERROR_TYPES,
} ValidatorErrorCodeT;

ValidatorErrorT *ValidatorGetLastError();
int32_t ValidatorIsErrorRaised();
void ValidatorRaiseError(ValidatorErrorCodeT code);
const char* ValidatorGetErrorMessageByCode(ValidatorErrorCodeT code);

#endif // __ERROR_H__
