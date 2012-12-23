#include <stdio.h>

#include "parser.h"

#include "tokenizer.h"
#include "error.h"
#include "utils.h"

// TODO:
// Separate parser and validator errors.
// Investigate fread return value issue.

static struct objRecord *ValidatorParseFormatDescription(char *format_description)
{
    struct objRecord *root = NULL;
    initialize(format_description);
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

ValidatorErrorT *ValidatorValidateString(char *data, char *format_description)
{
    struct objRecord *format = ValidatorParseFormatDescription(format_description);
    printf("%p\n", format);
    return ValidatorGetLastError();
}

ValidatorErrorT *ValidatorValidateFile(char *filename_data, char *filename_format)
{
    char *data = NULL;
    char *format = NULL;

    data = LoadTextFileIntoMemory(filename_data,
        E_CANNOT_READ_INPUT_FILE);
    format = LoadTextFileIntoMemory(filename_format,
        E_CANNOT_READ_FORMAT_DESCRIPTION_FILE);

    if (!ValidatorIsErrorRaised())
    {
        ValidatorValidateString(data, format);
    }

    free(data);
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
