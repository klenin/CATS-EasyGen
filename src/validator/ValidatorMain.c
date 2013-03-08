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

#define PARSER_CALL(call) do { call; ValidatorProcessParserError(); } while(0)

#define PARSER_ERROR_PREFIX "parser error: "

#define DECLARE_VALIDATOR(name) \
    static void name(ParserObjectT *object, ParserObjectRecordWithDataT *dataTree)

DECLARE_VALIDATOR(ValidatorValidateNewline);
DECLARE_VALIDATOR(ValidatorValidateSoftline);
DECLARE_VALIDATOR(ValidatorValidateInteger);
DECLARE_VALIDATOR(ValidatorValidateFloat);
DECLARE_VALIDATOR(ValidatorValidateString);
DECLARE_VALIDATOR(ValidatorValidateSequence);
DECLARE_VALIDATOR(ValidatorValidateEnd);

static void
    (*g_ValidatorFunctions[PARSER_OBJECT_KINDS_COUNT])
        (ParserObjectT*, ParserObjectRecordWithDataT*) =
{
    ValidatorValidateNewline,
    ValidatorValidateSoftline,
    ValidatorValidateInteger,
    ValidatorValidateFloat,
    ValidatorValidateString,
    ValidatorValidateSequence,
    ValidatorValidateEnd
};

static const ValidatorTokenizerTokenTypeT
    g_ParserObject2ValidatorToken[PARSER_OBJECT_KINDS_COUNT] =
{
    VTT_NEWLINE, // PARSER_OBJECT_KIND_NEWLINE
    0,           // PARSER_OBJECT_KIND_SOFTLINE
    VTT_INTEGER, // PARSER_OBJECT_KIND_INTEGER
    VTT_FLOAT,   // PARSER_OBJECT_KIND_FLOAT
    VTT_STRING,  // PARSER_OBJECT_KIND_STRING
    0,           // PARSER_OBJECT_KIND_SEQUENCE
    VTT_EOF      // PARSER_OBJECT_KIND_END
};

static void ValidatorProcessParserErrorEx(ParserErrorT *error)
{
    const char *parserMessage = ParserGetErrorMessageByCode(error->code);
    char *finalMessage;
    size_t messageLength =
        strlen(PARSER_ERROR_PREFIX) + strlen(parserMessage);

    finalMessage = AllocateBuffer(messageLength);
    sprintf(finalMessage, PARSER_ERROR_PREFIX"%s", parserMessage);
    ValidatorRaiseErrorEx(finalMessage, error->line, error->pos);
    throw(ParserException, finalMessage);
}

static void ValidatorProcessParserError()
{
    if (ParserIsErrorRaised())
    {
        ValidatorProcessParserErrorEx(ParserGetLastError());
    }
}

static ValidatorTokenizerTokenT *ValidatorSafeGetNextToken()
{
    ValidatorTokenizerTokenT *token = ValidatorTokenizerNextToken();
    if (token == NULL)
    {
        throw(ValidatorException, "unexpected end of data file");
    }
    return token;
}

static void ValidatorAssertObjectType(
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

DECLARE_VALIDATOR(ValidatorValidateNewline)
{
    ValidatorTokenizerTokenT *token = ValidatorSafeGetNextToken();
    ValidatorAssertObjectType(object->objKind, token->type);
    ValidatorTokenizerDestroyToken(token);
}

DECLARE_VALIDATOR(ValidatorValidateSoftline)
{
}

DECLARE_VALIDATOR(ValidatorValidateInteger)
{
    ValidatorTokenizerTokenT *token = ValidatorSafeGetNextToken();
    char *objectName;
    ParserObjectWithDataT objectData;
    int64_t leftBound, rightBound;
    int64_t objectValue;

    ValidatorAssertObjectType(object->objKind, token->type);
    objectName = object->attrList[PARSER_OBJECT_ATTR_NAME].strVal;
    PARSER_CALL(objectData = ParserFindObject(objectName, *dataTree, 0));
    PARSER_CALL(ParserEvaluateIntRange(objectData, &leftBound, &rightBound));
    objectValue = strtoll(token->text, NULL, 10);
    if (objectValue < leftBound || objectValue > rightBound)
    {
        throwf(ValidatorException, "%lld is not in range [%lld, %lld]",
            objectValue, leftBound, rightBound);
    }
    PARSER_CALL(ParserSetIntegerValue(objectData, objectValue));

    ValidatorTokenizerDestroyToken(token);
}

DECLARE_VALIDATOR(ValidatorValidateFloat)
{
}

DECLARE_VALIDATOR(ValidatorValidateString)
{
}

DECLARE_VALIDATOR(ValidatorValidateSequence)
{
}

DECLARE_VALIDATOR(ValidatorValidateEnd)
{
    // Dummy function.
}

static ParserObjectRecordT *ValidatorParseFormatDescription(
    char *formatDescription
)
{
    ParserObjectRecordT *tree = NULL;
    ParserErrorT *error = NULL;
    ParserValidateFormatDescription(formatDescription, &tree, &error);
    if (error != NULL)
    {
        ValidatorProcessParserErrorEx(error);
    }
    return tree;
}

ValidatorErrorT *ValidatorValidate(char *inputFilename, char *formatFilename)
{
    E4C_DECLARE_CONTEXT_STATUS

    FILE *inputHandle = NULL;
    char *formatText = NULL;
    ParserObjectT *currentObject = NULL;
    ParserObjectRecordT *formatTree = NULL;
    ParserObjectRecordIteratorT *formatIterator = NULL;
    ParserObjectRecordWithDataT *dataTree = NULL;

    E4C_REUSE_CONTEXT
    try
    {
        inputHandle = FileOpen(inputFilename, "r");
        ValidatorTokenizerSetInput(inputHandle);
        formatText = FileReadTextFile(formatFilename);
        formatTree = ValidatorParseFormatDescription(formatText);

        dataTree = AllocateBuffer(sizeof(ParserObjectRecordWithDataT));
        dataTree->pointerToData = NULL;
        dataTree->recPart = formatTree;
        PARSER_CALL(dataTree = ParserAllocateObjectRecordWithData(dataTree, 1));

        for (formatIterator = ParserObjectRecordGetFrontElement(formatTree);
                              ParserObjectRecordIteratorIsValid(formatIterator);
                              ParserObjectRecordIteratorAdvance(formatIterator))
        {
            currentObject = ParserObjectRecordIteratorDereference(formatIterator);
            g_ValidatorFunctions[currentObject->objKind](currentObject, dataTree);
        }
    }
    catch (ParserException)
    {
        // Validator error already raised, just catch.
    }
    catch (ValidatorException)
    {
        ValidatorRaiseErrorEx(
            e4c_get_exception()->message, yycurline(), yycurpos());
    }
    catch (RuntimeException)
    {
        ValidatorRaiseError(e4c_get_exception()->message);
    }
    finally
    {
        free(formatText);
        free(formatIterator);
        ParserDestroyObjectRecordWithData(dataTree);
        free(dataTree);
        ParserDestroyObjectRecord(formatTree);
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
