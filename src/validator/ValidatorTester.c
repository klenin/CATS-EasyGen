#include <stdio.h>
#include <stdlib.h>

#include "Validator.h"

int main(int argc, char **argv)
{
    ValidatorErrorT *error = NULL;
    char *inputFilename = NULL;
    char *formatFilename = NULL;

    if (argc < 3)
    {
        printf("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    inputFilename = argv[1];
    formatFilename = argv[2];
    error = ValidatorValidate(inputFilename, formatFilename);

    if (error != NULL)
    {
        printf("%d:%d %s\n", error->line, error->pos, error->message);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
