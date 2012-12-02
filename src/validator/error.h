#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdint.h>

typedef struct
{
    int32_t line;
    int32_t pos;
    int32_t code;
} validatorError;

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
} errorType;

struct _errordesc {
    int  code;
    char *message;
} error_messages[] = {
    { E_SUCCESS, "no error" },
    { E_CANNOT_READ_INPUT_FILE, "cannot read input file" },
    { E_CANNOT_READ_FORMAT_DESCRIPTION_FILE, "cannot read format description file" },
    { E_NOT_ENOUGH_MEMORY, "not enough memory" },
};

#endif // __ERROR_H__
