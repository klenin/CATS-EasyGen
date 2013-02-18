#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#include "ValidatorErrors.h"

ValidatorErrorT *ValidatorValidate(char *inputFilename, char *formatFilename);

#endif // __VALIDATOR_H__
