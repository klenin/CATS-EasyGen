#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "iterator.h"

#include "Allocator.h"
#include "File.h"
#include "ValidatorErrors.h"
#include "ValidatorExceptions.h"
#include "ValidatorTokenizer.h"

#define PARSER_ERROR_PREFIX "parser error: "

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

ValidatorErrorT *ValidatorValidateString(char *input, char *formatDescription)
{
    E4C_DECLARE_CONTEXT_STATUS

    ParserObjectRecordT *format = NULL;
    ParserObjectRecordIteratorT *it = NULL;
    ParserObjectT *object = NULL;

    E4C_REUSE_CONTEXT
    try
    {
        format = ValidatorParseFormatDescription(formatDescription);
    }
    catch (RuntimeException)
    {
        const e4c_exception *exception = e4c_get_exception();
        ValidatorRaiseError(exception->message);
    }
    E4C_CLOSE_CONTEXT

    if (ValidatorIsErrorRaised())
    {
        return ValidatorGetLastError();
    }

    for (it = ParserObjectRecordGetFrontElement(format);
              ParserObjectRecordIteratorIsValid(it);
              ParserObjectRecordIteratorAdvance(it))
    {
        object = ParserObjectRecordIteratorDereference(it);
        printf("objKind '%d'\n", object->objKind);
        printf("attrName '%s'\n", object->attrList[0].attrName);
        printf("strVal '%s'\n", object->attrList[0].strVal);
        printf("\n");
    }

    return ValidatorGetLastError();
}

ValidatorErrorT *ValidatorValidateFile(
    char *inputFilename,
    char *formatFilename
)
{
    E4C_DECLARE_CONTEXT_STATUS

    char *input = NULL;
    char *format = NULL;

    E4C_REUSE_CONTEXT
    try
    {
        input = LoadTextFileIntoMemory(inputFilename);
        format = LoadTextFileIntoMemory(formatFilename);
        ValidatorValidateString(input, format);
    }
    catch (RuntimeException)
    {
        const e4c_exception *exception = e4c_get_exception();
        ValidatorRaiseError(exception->message);
    }
    finally
    {
        free(input);
        free(format);
    }
    E4C_CLOSE_CONTEXT

    return ValidatorGetLastError();
}

int32_t main()
{
    ValidatorErrorT *error = ValidatorValidateFile("data", "formal.input");
    if (error != NULL)
    {
        printf("%d:%d %s\n", error->line, error->pos, error->message);
    }

    return 0;
}
