#include <stdio.h>
#include <stdlib.h>

#include "Allocator.h"
#include "Exceptions.h"

FILE *FileOpen(const char *filename, const char *mode)
{
    FILE *file = NULL;

    file = fopen(filename, mode);
    if (file == NULL)
    {
        throwf(InputOutputException, "cannot open file '%s'", filename);
    }

    return file;
}

char *FileReadTextFile(const char *filename)
{
    FILE *file = NULL;
    char *buffer = NULL;
    size_t filesize;
    size_t actual;

    file = FileOpen(filename, "r");
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
