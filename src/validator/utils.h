#ifndef __UTILS_H__
#define __UTILS_H__

#include "error.h"

void *AllocateBuffer(size_t size);
char *LoadTextFileIntoMemory(char *filename, ValidatorErrorCodeT errorCode);

#endif // __UTILS_H__
