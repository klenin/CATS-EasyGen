#include <stdio.h>
#include <stdlib.h>

#include "Allocator.h"
#include "Exceptions.h"
#include "LanguageStandard.h"

char *LoadTextFileIntoMemory(char *filename)
{
    FILE *file = NULL;
    char *buffer = NULL;
    size_t filesize;
    size_t actual;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        throwf(InputOutputException, "cannot read file '%s'", filename);
    }

    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    if (filesize == 0)
    {
        throwf(InputOutputException, "'%s': zero file size", filename);
    }
    else
    {
        rewind(file);
        buffer = AllocateBuffer(filesize);
    }

    actual = fread(buffer, sizeof(char), filesize, file);
    buffer[actual - 1] = '\0';
    fclose(file);

    return buffer;
}
