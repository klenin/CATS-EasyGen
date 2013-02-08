#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "iterator.h"

#include "tokenizer.h"
#include "error.h"
#include "utils.h"

// TODO:
// Separate parser and validator errors.
// Investigate fread return value issue.

static ParserObjectRecordT *ValidatorParseFormatDescription(
    char *formatDescription
)
{
    ParserObjectRecordT *tree = NULL;
    ParserErrorT *error = NULL;

    ParserValidateFormatDescription(formatDescription, &tree, &error);
    if (error != NULL && !ValidatorIsErrorRaised())
    {
        const char *parserMessage = ParserGetErrorMessageByCode(error->code);
        const char *validatorMessage;
        char *finalMessage;

        ValidatorRaiseErrorEx(E_PARSER_ERROR, error->line, error->pos);

        validatorMessage = ValidatorGetLastError()->message;
        finalMessage = AllocateBuffer(
            strlen(validatorMessage) + strlen(parserMessage));
        strcpy(finalMessage, validatorMessage);
        strncat(finalMessage, parserMessage, strlen(parserMessage));
        ValidatorGetLastError()->message = finalMessage;

        return NULL;
    }
    return tree;
}

ValidatorErrorT *ValidatorValidateString(char *input, char *formatDescription)
{
    ParserObjectRecordT *format = ValidatorParseFormatDescription(
        formatDescription);

    if (format != NULL)
    {
        ParserObjectRecordIteratorT *it = NULL;
        ParserObjectT *object = NULL;

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

    }

    return ValidatorGetLastError();
}

ValidatorErrorT *ValidatorValidateFile(
    char *inputFilename,
    char *formatFilename
)
{
    char *input = NULL;
    char *format = NULL;

    input = LoadTextFileIntoMemory(
        inputFilename, E_CANNOT_READ_INPUT_FILE);
    format = LoadTextFileIntoMemory(
        formatFilename, E_CANNOT_READ_FORMAT_DESCRIPTION_FILE);

    if (!ValidatorIsErrorRaised())
    {
        ValidatorValidateString(input, format);
    }

    free(input);
    free(format);

    return ValidatorGetLastError();
}

int32_t main()
{
    ValidatorErrorT *error = ValidatorValidateFile("data", "formal.input");
    if (error != NULL)
    {
        printf("%d:%d %s\n", error->line, error->pos, error->message);
        free(error->message);
    }

    return 0;
}
