#include <stdio.h>
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
    ParserObjectRecordT *formatTree = NULL;
    ValidatorTokenizerTokenT *currentToken = NULL;

    E4C_REUSE_CONTEXT
    try
    {
        inputHandle = fopen(inputFilename, "r");
        if (inputHandle == NULL)
        {
            throw(InputOutputException, "cannot open input file");
        }
        ValidatorTokenizerSetInput(inputHandle);

        formatText = LoadTextFileIntoMemory(formatFilename);
        formatTree = ValidatorParseFormatDescription(formatText);

        do
        {
            free(currentToken);
            currentToken = ValidatorTokenizerNextToken();
            if (currentToken->type == VTT_INTEGER)
            {
                printf("'%s' == %I64d\n", currentToken->text,
                    ConvertStringToInt64(currentToken->text));
            }
        } while (currentToken->type != VTT_EOF);
    }
    catch (RuntimeException)
    {
        const e4c_exception *exception = e4c_get_exception();
        ValidatorRaiseError(exception->message);
    }
    finally
    {
        free(formatText);
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
