#include <stdlib.h>

#include "Exceptions.h"
#include "LanguageStandard.h"

void *AllocateBuffer(size_t size)
{
    void *memory = malloc(size);
    if (memory == NULL)
    {
#ifdef C99
        throwf(NotEnoughMemoryException, "could not allocate %u bytes", size);
#else
        throw(NotEnoughMemoryException, "not enough memory");
#endif
    }
    return memory;
}
