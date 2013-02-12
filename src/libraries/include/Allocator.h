#ifndef __LIB_ALLOCATOR_H__
#define __LIB_ALLOCATOR_H__

#include <stdlib.h>

void *AllocateBuffer(size_t size);
void *ReallocateBuffer(void *buffer, size_t size);
void *AllocateArray(size_t count, size_t size);

#endif // __LIB_ALLOCATOR_H__
