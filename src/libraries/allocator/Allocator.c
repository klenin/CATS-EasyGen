#include <stdlib.h>

#include "Exceptions.h"
#include "LanguageStandard.h"

static void CheckAllocation(void *buffer, size_t size)
{
    if (buffer == NULL)
    {
        throwf(NotEnoughMemoryException, "could not allocate %u bytes", size);
    }
}

void *AllocateBuffer(size_t size)
{
    void *buffer = malloc(size);
    CheckAllocation(buffer, size);
    return buffer;
}

void *ReallocateBuffer(void *buffer, size_t size)
{
    void *newBuffer = realloc(buffer, size);
    CheckAllocation(newBuffer, size);
    return newBuffer;
}

void *AllocateArray(size_t count, size_t size)
{
    void *buffer = calloc(count, size);
    CheckAllocation(buffer, size);
    return buffer;
}
