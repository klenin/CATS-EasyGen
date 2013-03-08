#ifndef __VALIDATOR_EXCEPTIONS_H__
#define __VALIDATOR_EXCEPTIONS_H__

#include "Exceptions.h"

E4C_DEFINE_EXCEPTION(ValidatorException, "Validator exception", RuntimeException);
E4C_DEFINE_EXCEPTION(ParserException, "Parser exception", RuntimeException);

#endif // __VALIDATOR_EXCEPTIONS_H__
