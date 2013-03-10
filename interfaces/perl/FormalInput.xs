#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <CatsFormalInput.h>

#include "const-c.inc"

MODULE = CATS::FormalInput    PACKAGE = CATS::FormalInput

PROTOTYPES: DISABLE

INCLUDE: const-xs.inc

ParserErrorT*
ParserValidateFormatDescription(formatDescription)
    const char* formatDescription

const char*
ParserGetErrorMessage(error)
    ParserErrorT* error

int
ParserGetErrorLine(error)
    ParserErrorT* error

int
ParserGetErrorPos(error)
    ParserErrorT* error

ValidatorErrorT*
ValidatorValidate(inputFilename, formatDescription)
    char* inputFilename
    char* formatDescription

const char*
ValidatorErrorGetErrorMessage(error)
    ValidatorErrorT* error

int
ValidatorErrorGetErrorLine(error)
    ValidatorErrorT* error

int
ValidatorErrorGetErrorPos(error)
    ValidatorErrorT* error
