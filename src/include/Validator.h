#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#include "Parser.h"
#include "ValidatorErrors.h"

ParserObjectRecordWithDataT *ValidatorValidate(
    char *inputFilename,
    char *formatDescription
);

#endif // __VALIDATOR_H__
