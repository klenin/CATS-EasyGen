#include <stdio.h>

#include "parser.h"

#include "tokenizer.h"
#include "error.h"
#include "utils.h"

// TODO:
// Separate parser and validator errors.
// Investigate fread return value issue.

static struct objRecord *ValidatorParseFormatDescription(
    char *formatDescription
)
{
    struct objRecord *root = NULL;
    initialize(formatDescription);
    root = parseObjRecord();
    if (wasError())
    {
        struct parseError *error = getLastError();
        printf("Got parser error\n");
        // Handle parser error;
    }
    finalize();
    return root;
}

ValidatorErrorT *ValidatorValidateString(char *input, char *formatDescription)
{
    struct objRecord *format = ValidatorParseFormatDescription(
        formatDescription);
    printf("%p\n", format);
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
        printf("%s\n", ValidatorGetErrorMessageByCode(error->code));
    }

    return 0;
}
