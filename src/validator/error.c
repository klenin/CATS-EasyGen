#include "utils.h"
#include "error.h"

typedef struct
{
    ValidatorErrorCodeT code;
    char *message;
} ValidatorErrorMessagesListT;

static ValidatorErrorT *LastError = NULL;

static ValidatorErrorMessagesListT gErrorMessages[] = {
    { E_SUCCESS, "no error" },
    { E_CANNOT_READ_INPUT_FILE, "cannot read input file" },
    { E_CANNOT_READ_FORMAT_DESCRIPTION_FILE, "cannot read format description file" },
    { E_NOT_ENOUGH_MEMORY, "not enough memory" },
};

ValidatorErrorT *ValidatorGetLastError()
{
    return LastError;
}

int32_t ValidatorIsErrorRaised()
{
    return LastError != NULL;
}

void ValidatorRaiseError(ValidatorErrorCodeT code)
{
    if (LastError == NULL)
    {
        LastError = AllocateBuffer(sizeof(ValidatorErrorCodeT));
        LastError->code = code;
        //LastError->line = line;
        //LastError->pos = pos;
    }
}

const char* ValidatorGetErrorMessageByCode(ValidatorErrorCodeT code)
{
    if (code >= E_ERROR_RANGE_START && code < E_NUMBER_OF_ERROR_TYPES) {
        int i;
        for (i = E_ERROR_RANGE_START; i < E_NUMBER_OF_ERROR_TYPES; ++i) {
            if (gErrorMessages[i].code == code) {
                return gErrorMessages[i].message;
            }
        }
    }
    return NULL;
}
