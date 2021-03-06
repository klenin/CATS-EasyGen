#include <ctype.h>
#include <errno.h>
#include <limits.h>
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

#define PARSER_CALL(call) do { call; ProcessParserError(); } while(0)

#define PARSER_ERROR_PREFIX "parser error: "

#define DECLARE_VALIDATOR(name) \
    static void name(ParserObjectT *object, ParserObjectRecordWithDataT *dataTree)

DECLARE_VALIDATOR(ValidateNewline);
DECLARE_VALIDATOR(ValidateSoftline);
DECLARE_VALIDATOR(ValidateGenericObject);
DECLARE_VALIDATOR(ValidateSequence);
DECLARE_VALIDATOR(ValidateEnd);

static void
    (*g_ValidatorFunctions[PARSER_OBJECT_KINDS_COUNT])
        (ParserObjectT*, ParserObjectRecordWithDataT*) =
{
    ValidateNewline,
    ValidateSoftline,
    ValidateGenericObject,
    ValidateGenericObject,
    ValidateGenericObject,
    ValidateSequence,
    ValidateEnd
};

static const ValidatorTokenizerTokenTypeT
    g_ParserObject2ValidatorToken[PARSER_OBJECT_KINDS_COUNT] =
{
    VTT_NEWLINE, // PARSER_OBJECT_KIND_NEWLINE
    0,           // PARSER_OBJECT_KIND_SOFTLINE (dummy value)
    VTT_INTEGER, // PARSER_OBJECT_KIND_INTEGER
    VTT_FLOAT,   // PARSER_OBJECT_KIND_FLOAT
    VTT_STRING,  // PARSER_OBJECT_KIND_STRING
    0,           // PARSER_OBJECT_KIND_SEQUENCE (dummy value)
    VTT_EOF      // PARSER_OBJECT_KIND_END
};

static void ProcessParserErrorEx(ParserErrorT *error)
{
    const char *parserMessage = ParserGetErrorMessageByCode(error->code);
    char *finalMessage;
    size_t messageLength =
        strlen(PARSER_ERROR_PREFIX) + strlen(parserMessage);

    finalMessage = AllocateBuffer(messageLength + 1);
    sprintf(finalMessage, PARSER_ERROR_PREFIX"%s", parserMessage);
    ValidatorRaiseErrorEx(finalMessage, error->line, error->pos);
    free(error);
    throw(ParserException, finalMessage);
}

static void ProcessParserError()
{
    if (ParserIsErrorRaised())
    {
        ProcessParserErrorEx(ParserGetLastError());
    }
}

static ValidatorTokenizerTokenT *NextToken()
{
    ValidatorTokenizerTokenT *token = ValidatorTokenizerNextToken();
    if (token == NULL)
    {
        throw(ValidatorException, "unexpected end of data file");
    }
    return token;
}

static void AssertObjKind(
    ParserObjectKindT expected,
    ValidatorTokenizerTokenTypeT actual
)
{
    if (g_ParserObject2ValidatorToken[expected] != actual)
    {
        throwf(ValidatorException, "expected %s, but got %s",
            g_ParserObjectKind2Str[expected],
            g_ValidatorTokenizerTokenType2Str[actual]);
    }
}

DECLARE_VALIDATOR(ValidateNewline)
{
    ValidatorTokenizerTokenT *token = NextToken();
    AssertObjKind(object->objKind, token->type);
    ValidatorTokenizerDestroyToken(token);
}

DECLARE_VALIDATOR(ValidateSoftline)
{
    ValidatorTokenizerTokenT *token = NextToken();
    if (VTT_NEWLINE == token->type)
    {
        // Skip newline.
        ValidatorTokenizerDestroyToken(token);
    }
    else
    {
        // No newline found, rewind tokenizer.
        ValidatorTokenizerRewind();
    }
}

static void InternalValidateInteger(
    ValidatorTokenizerTokenT *token,
    ParserObjectWithDataT objectData
)
{
    int64_t leftBound, rightBound;
    int64_t objectValue;

    objectValue = strtoll(token->text, NULL, 10);
    if (errno == ERANGE)
    {
        throwf(ValidatorException,
            (objectValue == LLONG_MAX ?
                "too big integer value %s":
                "too small integer value %s"),
            token->text);
    }

    PARSER_CALL(ParserEvaluateIntRange(objectData, &leftBound, &rightBound));
    if (objectValue < leftBound || objectValue > rightBound)
    {
        throwf(ValidatorException,
            "%" PRId64 " is not in range [%" PRId64 ", %" PRId64 "]",
            objectValue, leftBound, rightBound);
    }

    PARSER_CALL(ParserSetIntegerValue(objectData, objectValue));
}

static void InternalValidateFloat(
    ValidatorTokenizerTokenT *token,
    ParserObjectWithDataT objectData
)
{
    int64_t leftBound, rightBound;
    long double objectValue;

    if (!ConvertStringToLongDouble(token->text, &objectValue))
    {
        throwf(ValidatorException, "cannot convert %s to long double value",
            token->text);
    }

    PARSER_CALL(ParserEvaluateIntRange(objectData, &leftBound, &rightBound));
    if (objectValue < leftBound || objectValue > rightBound)
    {
        throwf(ValidatorException,
            "%s is not in range [%" PRId64 ", %" PRId64 "]",
            token->text, leftBound, rightBound);
    }

    PARSER_CALL(ParserSetFloatValue(objectData, objectValue));
}

static void InternalValidateString(
    ValidatorTokenizerTokenT *token,
    ParserObjectWithDataT objectData
)
{
    char *objectValue = NULL;
    char *validCharacters = NULL;
    int64_t leftBound, rightBound;
    int64_t valueLength = strlen(token->text);
    int32_t i;

    PARSER_CALL(ParserEvaluateLenRange(objectData, &leftBound, &rightBound));
    if (valueLength < leftBound || valueLength > rightBound)
    {
        throwf(ValidatorException,
            "string length %" PRId64 " is not in range [%" PRId64 ", %" PRId64 "]",
            valueLength, leftBound, rightBound);
    }

    validCharacters = objectData.objPart->attrList[PARSER_OBJECT_ATTR_CHARS].valid;
    for (i = 0; i < valueLength; ++i)
    {
        unsigned char character = token->text[i];
        if (!validCharacters[character])
        {
            throwf(ValidatorException,
                (isprint(character) ?
                    "invalid character '%c' on position %u":
                    "invalid character \\%u on position %u"),
                character, i + 1);
        }
    }

    objectValue = token->text;
    PARSER_CALL(ParserSetStringValue(objectData, objectValue));
}

DECLARE_VALIDATOR(ValidateGenericObject)
{
    ValidatorTokenizerTokenT *token = NextToken();
    char *objectName;
    ParserObjectWithDataT objectData;

    AssertObjKind(object->objKind, token->type);
    objectName = object->attrList[PARSER_OBJECT_ATTR_NAME].strVal;
    PARSER_CALL(objectData = ParserFindObjectByName(*dataTree, objectName));

    switch (object->objKind)
    {
        case PARSER_OBJECT_KIND_INTEGER:
        {
            InternalValidateInteger(token, objectData);
            break;
        }
        case PARSER_OBJECT_KIND_FLOAT:
        {
            InternalValidateFloat(token, objectData);
            break;
        }
        case PARSER_OBJECT_KIND_STRING:
        {
            InternalValidateString(token, objectData);
            break;
        }
        default:
        {
            throwf(ValidatorException, "unexpected %s",
                g_ParserObjectKind2Str[object->objKind]);
        }
    }

    ValidatorTokenizerDestroyToken(token);
}

static void ValidateObjectRecord(ParserObjectRecordWithDataT *recordData)
{
    ParserObjectT *currentObject = NULL;
    ParserObjectRecordIteratorT *iterator = NULL;

    try
    {
        for (iterator = ParserObjectRecordGetFrontElement(recordData->recPart);
                        ParserObjectRecordIteratorIsValid(iterator);
                        ParserObjectRecordIteratorAdvance(iterator))
        {
            currentObject = ParserObjectRecordIteratorDereference(iterator);
            g_ValidatorFunctions[currentObject->objKind](currentObject, recordData);
        }
    }
    finally
    {
        free(iterator);
    }
}

DECLARE_VALIDATOR(ValidateSequence)
{
    char *objectName;
    ParserObjectWithDataT objectData;
    int32_t sequenceLength;
    int32_t i;

    objectName = object->attrList[PARSER_OBJECT_ATTR_NAME].strVal;
    PARSER_CALL(objectData = ParserFindObjectByName(*dataTree, objectName));
    PARSER_CALL(sequenceLength = ParserGetSequenceLength(objectData));

    for (i = 1; i <= sequenceLength; ++i)
    {
        ParserObjectRecordWithDataT item;
        PARSER_CALL(item = ParserGetSequenceElement(objectData, i));
        ValidateObjectRecord(&item);
    }
}

DECLARE_VALIDATOR(ValidateEnd)
{
    // Dummy function.
}

static ParserObjectRecordT *ParseFormatDescription(
    char *formatDescription
)
{
    ParserObjectRecordT *tree = NULL;
    ParserErrorT *error = ParserValidateFormatDescriptionEx(
        formatDescription, &tree);

    if (error != NULL)
    {
        ProcessParserErrorEx(error);
    }

    return tree;
}

ParserObjectRecordWithDataT *ValidatorBuildDataTree(
    char *inputFilename,
    char *formatDescription
)
{
    E4C_BOOL isUncaught = E4C_FALSE;
    FILE *inputHandle = NULL;
    ParserObjectRecordT *formatTree = NULL;
    ParserObjectRecordWithDataT *dataTree = NULL;

    // Need to create an exception context before catching exceptions.
    e4c_reusing_context(isUncaught, E4C_TRUE)
    {
        try
        {
            inputHandle = FileOpen(inputFilename, "r");
            ValidatorTokenizerSetInput(inputHandle);
            formatTree = ParseFormatDescription(formatDescription);
            dataTree = AllocateBuffer(sizeof(ParserObjectRecordWithDataT));
            dataTree->pointerToData = NULL;
            dataTree->recPart = formatTree;
            PARSER_CALL(
                dataTree = ParserAllocateObjectRecordWithData(dataTree, 1));
            ValidateObjectRecord(dataTree);
        }
        catch (ParserException)
        {
            // Validator error already raised, just catch.
        }
        catch (ValidatorException)
        {
            ValidatorRaiseErrorEx(
                e4c_get_exception()->message,
                ValidatorTokenizerGetCurrentLine(),
                ValidatorTokenizerGetCurrentPosition());
        }
        catch (RuntimeException)
        {
            ValidatorRaiseError(e4c_get_exception()->message);
        }
        finally
        {
            if (inputHandle != NULL)
            {
                fclose(inputHandle);
            }
        }
    }

    if (ValidatorIsErrorRaised())
    {
        ParserDestroyObjectRecordWithData(dataTree);
        free(dataTree);
        ParserDestroyObjectRecord(formatTree);
        return NULL;
    }

    return dataTree;
}

/*******************************************************************************
                            CATS interface functions
*******************************************************************************/

ValidatorErrorT *ValidatorValidate(char *inputFilename, char *formatDescription)
{
    ParserObjectRecordWithDataT *data = ValidatorBuildDataTree(
        inputFilename, formatDescription);
    ParserDestroyObjectRecordWithData(data);
    free(data);
    return ValidatorGetLastError();
}

const char *ValidatorErrorGetErrorMessage(ValidatorErrorT *error)
{
    return error->message;
}

int ValidatorErrorGetErrorLine(ValidatorErrorT *error)
{
    return error->line;
}

int ValidatorErrorGetErrorPos(ValidatorErrorT *error)
{
    return error->pos;
}
