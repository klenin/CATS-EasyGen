#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Allocator.h"
#include "File.h"
#include "Numerics.h"
#include "Parser.h"
#include "ParserObjectRecordIterator.h"
#include "ValidatorErrors.h"
#include "ValidatorExceptions.h"
#include "ValidatorTokenizer.h"

#define PARSER_ERROR_PREFIX "parser error: "

#define DECLARE_VALIDATOR(name) static int name(ParserObjectT *object)

DECLARE_VALIDATOR(ValidatorValidateNewline);
DECLARE_VALIDATOR(ValidatorValidateSoftline);
DECLARE_VALIDATOR(ValidatorValidateInteger);
DECLARE_VALIDATOR(ValidatorValidateFloat);
DECLARE_VALIDATOR(ValidatorValidateString);
DECLARE_VALIDATOR(ValidatorValidateSequence);
DECLARE_VALIDATOR(ValidatorValidateEnd);

static int (*g_ValidatorFunctions[PARSER_OBJECT_KINDS_COUNT])(ParserObjectT*) =
{
    ValidatorValidateNewline,
    ValidatorValidateSoftline,
    ValidatorValidateInteger,
    ValidatorValidateFloat,
    ValidatorValidateString,
    ValidatorValidateSequence,
    ValidatorValidateEnd
};

static const ValidatorTokenizerTokenTypeT
    g_ParserObject2ValidatorToken[PARSER_OBJECT_KINDS_COUNT] =
{
    VTT_NEWLINE, // PARSER_OBJECT_KIND_NEWLINE
    0,           // PARSER_OBJECT_KIND_SOFTLINE
    VTT_INTEGER, // PARSER_OBJECT_KIND_INTEGER
    VTT_FLOAT,   // PARSER_OBJECT_KIND_FLOAT
    VTT_STRING,  // PARSER_OBJECT_KIND_STRING
    0,           // PARSER_OBJECT_KIND_SEQUENCE
    VTT_EOF      // PARSER_OBJECT_KIND_END
};

static ValidatorTokenizerTokenT *ValidatorSafeGetNextToken()
{
    ValidatorTokenizerTokenT *token = ValidatorTokenizerNextToken();
    if (token == NULL)
    {
        throw(ValidatorException, "unexpected end of data file");
    }
    return token;
}

static void ValidatorAssertObjectType(
    ParserObjectKindT expected,
    ValidatorTokenizerTokenTypeT actual
)
{
    if (g_ParserObject2ValidatorToken[expected] != actual)
    {
        throwf(ValidatorException, "expected '%s', but got '%s'",
            g_ParserObjectKind2Str[expected],
            g_ValidatorTokenizerTokenType2Str[actual]);
    }
}

DECLARE_VALIDATOR(ValidatorValidateNewline)
{
    ValidatorTokenizerTokenT *token = ValidatorSafeGetNextToken();
    ValidatorAssertObjectType(object->objKind, token->type);
    ValidatorTokenizerDestroyToken(token);
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateSoftline)
{
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateInteger)
{
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateFloat)
{
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateString)
{
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateSequence)
{
    return EXIT_SUCCESS;
}

DECLARE_VALIDATOR(ValidatorValidateEnd)
{
    return EXIT_SUCCESS;
}

static ParserObjectRecordT *ValidatorParseFormatDescription(
    char *formatDescription
)
{
    ParserObjectRecordT *tree = NULL;
    ParserErrorT *error = NULL;

    ParserValidateFormatDescription(formatDescription, &tree, &error);
    if (error != NULL)
    {
        const char *parserMessage = ParserGetErrorMessageByCode(error->code);
        char *finalMessage;
        size_t messageLength =
            strlen(PARSER_ERROR_PREFIX) + strlen(parserMessage);

        finalMessage = AllocateBuffer(messageLength);
        sprintf(finalMessage, PARSER_ERROR_PREFIX"%s", parserMessage);
        ValidatorRaiseErrorEx(finalMessage, error->line, error->pos);
    }
    return tree;
}

ValidatorErrorT *ValidatorValidate(char *inputFilename, char *formatFilename)
{
    E4C_DECLARE_CONTEXT_STATUS

    FILE *inputHandle = NULL;
    char *formatText = NULL;
    ParserObjectT *currentObject = NULL;
    ParserObjectRecordT *formatTree = NULL;
    ParserObjectRecordIteratorT *formatIterator = NULL;

    E4C_REUSE_CONTEXT
    try
    {
        inputHandle = FileOpen(inputFilename, "r");
        ValidatorTokenizerSetInput(inputHandle);

        formatText = FileReadTextFile(formatFilename);
        formatTree = ValidatorParseFormatDescription(formatText);
        for (formatIterator = ParserObjectRecordGetFrontElement(formatTree);
                              ParserObjectRecordIteratorIsValid(formatIterator);
                              ParserObjectRecordIteratorAdvance(formatIterator))
        {
            currentObject = ParserObjectRecordIteratorDereference(formatIterator);
            g_ValidatorFunctions[currentObject->objKind](currentObject);
        }
    }
    catch (RuntimeException)
    {
        const e4c_exception *exception = e4c_get_exception();
        ValidatorRaiseError(exception->message);
    }
    finally
    {
        free(formatText);
        free(formatIterator);
        if (inputHandle != NULL)
        {
            fclose(inputHandle);
        }
    }
    E4C_CLOSE_CONTEXT

    if (E4C_WAS_EXCEPTION_THROWN() || ValidatorIsErrorRaised())
    {
        return ValidatorGetLastError();
    }
    return NULL;
}
