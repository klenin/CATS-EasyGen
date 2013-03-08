#include <stdio.h>
#include <stdlib.h>

#include "File.h"
#include "Parser.h"
#include "Validator.h"

int main(int argc, char **argv)
{
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

    formatDescription = FileReadTextFile(formatFilename);
    data = ValidatorValidate(inputFilename, formatDescription);
    error = ValidatorGetLastError();

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
