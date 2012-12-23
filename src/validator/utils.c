#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void *AllocateBuffer(size_t size)
{
    void *memory = malloc(size);
    if (memory == NULL)
    {
        ValidatorRaiseError(E_NOT_ENOUGH_MEMORY);
    }
    return memory;
}


char *LoadTextFileIntoMemory(char *filename, ValidatorErrorCodeT errorCode)
{
    FILE *file = NULL;
    char *buffer = NULL;
    size_t filesize;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        ValidatorRaiseError(errorCode);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    rewind(file);
    buffer = AllocateBuffer(filesize);
    if (!ValidatorIsErrorRaised())
    {
        size_t actual = fread(buffer, sizeof(char), filesize, file);
        buffer[actual - 1] = '\0';
        fclose(file);
    }
    return buffer;
}
