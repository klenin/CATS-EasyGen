#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>

#include "Numerics.h"
#include "ParserObjectAttributes.h"
#include "Random.h"

struct expr
{
    char opCode;
    int64_t intConst;
    char* varName;
    struct expr *op1,*op2;
};

typedef struct
{
    char* attrName;
    char* strVal;
    struct expr *exVal1, *exVal2;
    int charNum;
    char valid[256];
    char* charSetStr;
} ParserObjectAttrT;

typedef struct
{
    ParserObjectKindT objKind;
    ParserObjectAttrT* attrList;
    struct ParserInternalObjectRecordT *rec, *parent;
} ParserObjectT;

typedef struct
{
    struct ParserInternalObjectRecordDataT* parentData;
    void* value;
} ParserObjectDataT;

typedef struct ParserInternalObjectRecordDataT
{
    ParserObjectDataT *data, *parentData;
} ParserObjectRecordDataT;

typedef struct ParserInternalObjectRecordT
{
    int n;
    ParserObjectT* seq;
    struct ParserInternalObjectRecordT* parent;
} ParserObjectRecordT;

typedef struct
{
    ParserObjectRecordT* recPart;
    ParserObjectRecordDataT* pointerToData;
} ParserObjectRecordWithDataT;

typedef struct
{
    ParserObjectT* objPart;
    ParserObjectDataT* pointerToData;
} ParserObjectWithDataT;

typedef struct
{
    int line, pos;
    int code;
} ParserErrorT;

// all destructors free memory for object and it's internal structures
void exprDestructor(struct expr* a);
void attrDestructor(ParserObjectAttrT* a);
void objDestructor(ParserObjectT* a);
void ParserDestroyObjectRecord(ParserObjectRecordT* a);
void parseErrorDestructor(ParserErrorT* a);

// utilities
int isInCharSet(char ch, char* desc); // return positive integer if in,
                                                  // negative if error, zero if not in
ParserErrorT* ParserGetLastError(); // (!) copy global structure to result,
int ParserIsErrorRaised();
const char* errorMessageByCode(int errCode);
ParserObjectWithDataT ParserFindObject(
    const char* name,
    ParserObjectRecordWithDataT rec,
    int goUp
);
int ParserGetSequenceLength(ParserObjectWithDataT info);

// important functions
void initialize(const char* buf);
void finalize();
ParserObjectRecordT* parseObjRecord(); // (!)

void ParserValidateFormatDescriptionEx(
    const char* data,
    ParserObjectRecordT** tree,
    ParserErrorT** error
); // (!)

void ParserPrintDataRecord(ParserObjectRecordWithDataT *recData);

const char* ParserGetErrorMessageByCode(int errCode);

// mainly for cats web-server
ParserErrorT *ParserValidateFormatDescription(const char *formatDescription);
const char *ParserGetErrorMessage(ParserErrorT *error);
int ParserGetErrorLine(ParserErrorT *error);
int ParserGetErrorPos(ParserErrorT *error);

ParserObjectRecordWithDataT* ParserAllocateObjectRecordWithData(
    ParserObjectRecordWithDataT *info,
    int isRoot
);
void ParserEvaluateIntRange(ParserObjectWithDataT info, int64_t* l, int64_t* r);
void ParserEvaluateLenRange(ParserObjectWithDataT info, int64_t* l, int64_t* r);
int64_t evaluate(struct expr* e, ParserObjectWithDataT info);

int64_t ParserGetFloatDigits(ParserObjectWithDataT info);
int64_t ParserGetIntegerValue(ParserObjectWithDataT info);
long double ParserGetFloatValue(ParserObjectWithDataT info);
char* ParserGetStringValue(ParserObjectWithDataT info); // just a pointer, no copy

void ParserSetIntegerValue(ParserObjectWithDataT info, const int64_t value);
void ParserSetFloatValue(ParserObjectWithDataT info, const long double value);
void ParserSetStringValue(ParserObjectWithDataT info, const char* value); // copy value

ParserObjectRecordWithDataT ParserGetSequenceElement(
    ParserObjectWithDataT info,
    int64_t index
);
ParserObjectWithDataT ParserFindObjectByName(
    ParserObjectRecordWithDataT info,
    const char* name
);

void autoGenRecord(ParserObjectRecordWithDataT info);
void autoGenSeq(ParserObjectWithDataT info);
void autoGenInt(ParserObjectWithDataT info);
void autoGenFloat(ParserObjectWithDataT info);
void autoGenStr(ParserObjectWithDataT info);

void ParserDestroyObjectRecordWithData(ParserObjectRecordWithDataT *info);
void destroySeqData(ParserObjectWithDataT info);

// user should destroy stuctures, returned by procedures marked with (!),
// user should also destroy all data by destroy...Data procedures

#endif // __PARSER_H__
