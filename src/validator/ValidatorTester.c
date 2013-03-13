#include <stdio.h>
#include <stdlib.h>

#include "Exceptions.h"
#include "File.h"
#include "Parser.h"
#include "Validator.h"

int main(int argc, char **argv)
{
    E4C_BOOL isUncaught = E4C_FALSE;
    ValidatorErrorT *error = NULL;
    char *inputFilename = NULL;
    char *formatFilename = NULL;
    char *formatDescription = NULL;
    ParserObjectRecordWithDataT *data = NULL;

    if (argc < 3)
    {
        printf("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    inputFilename = argv[1];
    formatFilename = argv[2];

    e4c_reusing_context(isUncaught, E4C_TRUE)
    {
        try
        {
            formatDescription = FileReadTextFile(formatFilename);
            data = ValidatorBuildDataTree(inputFilename, formatDescription);
            error = ValidatorGetLastError();
        }
        catch (RuntimeException)
        {
            printf(e4c_get_exception()->message);
        }
    }

    if (error != NULL)
    {
        if (error->line > 0 && error->pos >= 0)
        {
            printf("%d:%d %s\n", error->line, error->pos, error->message);
        }
        else
        {
            printf("%s\n", error->message);
        }
        return EXIT_FAILURE;
    }

    ParserPrintDataRecord(data);

    return EXIT_SUCCESS;
}
