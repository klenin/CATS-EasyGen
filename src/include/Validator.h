#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#include "Parser.h"
#include "ValidatorErrors.h"

ValidatorErrorT *ValidatorValidate(
    char *inputFilename,
    char *formatDescription
);

ParserObjectRecordWithDataT *ValidatorBuildDataTree(
    char *inputFilename,
    char *formatDescription
);

#endif // __VALIDATOR_H__
