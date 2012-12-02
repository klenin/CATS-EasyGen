#include <stdio.h>

#include "error.h"
#include "parser.h"
#include "tokenizer.h"

// TODO:
// Separate parser and validator errors.
// Investigate fread return value issue.

validatorError *LastError = NULL;

static void *allocate(size_t size);

static validatorError *get_last_error()
{
    return LastError;
}

static int32_t raised()
{
    return LastError != NULL;
}

static void raise(errorType type)
{
    if (LastError == NULL)
    {
        LastError = allocate(sizeof(validatorError));
        LastError->code = type;
        LastError->line = yycurline() | -1;
        LastError->pos = yycurpos() | -1;
    }
}

static void *allocate(size_t size)
{
    void *memory = malloc(size);
    if (memory == NULL)
    {
        raise(E_NOT_ENOUGH_MEMORY);
    }
    return memory;
}

static char *read_whole(char *filename, errorType error)
{
    FILE *file = NULL;
    char *buffer = NULL;
    size_t filesize;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        raise(error);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    rewind(file);
    buffer = allocate(filesize);
    if (!raised())
    {
        size_t actual = fread(buffer, sizeof(char), filesize, file);
        buffer[actual - 1] = '\0';
        fclose(file);
    }
    return buffer;
}

static struct objRecord *parse_format(char *format_description)
{
    struct objRecord *root = NULL;
    struct parseError *error = NULL;
    initialize(format_description);
    root = parseObjRecord();
    if (wasError())
    {
        error = getLastError();
        printf("Got parser error\n");
        // Handle parser error;
    }
    finalize();
    return root;
}

validatorError *validate(char *data, char *format_description)
{
    struct objRecord *format = parse_format(format_description);
    printf("%p\n", format);
    return get_last_error();
}

validatorError *fvalidate(char *filename_data, char *filename_format)
{
    char *data = NULL;
    char *format = NULL;

    data = read_whole(filename_data, E_CANNOT_READ_INPUT_FILE);
    format = read_whole(filename_format, E_CANNOT_READ_FORMAT_DESCRIPTION_FILE);

    if (!raised())
    {
        printf("Validate now\n");
        validate(data, format);
    }

    free(data);
    free(format);

    return get_last_error();
}

// TEMPORARY

static const char* error_message_by_code(int code)
{
    if (code >= E_ERROR_RANGE_START && code < E_NUMBER_OF_ERROR_TYPES) {
        int i;
        for (i = E_ERROR_RANGE_START; i < E_NUMBER_OF_ERROR_TYPES; ++i) {
            if (error_messages[i].code == code) {
                return error_messages[i].message;
            }
        }
    }
    return NULL;
}

int32_t main()
{
    /*freopen("formal.input", "r", stdin);

    do
    {
        code = yylex();
        printf("%d: '%s' (%u chars)\n", code, yytoktext(), yytoklen());
    } while (code != ttEof);

    fclose(stdin);*/
    validatorError *error = fvalidate("a", "formal.input");
    if (error != NULL)
    {
        printf("%s\n", error_message_by_code(error->code));
    }

    return 0;
}
