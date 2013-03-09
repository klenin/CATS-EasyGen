#include <string.h>

#include "Allocator.h"
#include "ValidatorErrors.h"

static ValidatorErrorT *LastError = NULL;

ValidatorErrorT *ValidatorGetLastError()
{
    return LastError;
}

int32_t ValidatorIsErrorRaised()
{
    return LastError != NULL;
}

void ValidatorRaiseError(const char *message)
{
    if (!ValidatorIsErrorRaised())
    {
        LastError = AllocateBuffer(sizeof(ValidatorErrorT));
        LastError->message = AllocateBuffer(strlen(message) + 1);
        strcpy(LastError->message, message);
        LastError->line = -1;
        LastError->pos = -1;
    }
}

void ValidatorRaiseErrorEx(const char *message, int32_t line, int32_t pos)
{
    if (!ValidatorIsErrorRaised())
    {
        ValidatorRaiseError(message);
        LastError->line = line;
        LastError->pos = pos;
    }
}
