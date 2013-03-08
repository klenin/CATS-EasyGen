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

struct objData
{
    struct recordData* parentData;
    void* value;
};

struct recordData
{
    struct objData *data, *parentData;
};


typedef struct ParserInternalObjectRecordT
{
    int n;
    ParserObjectT* seq;
    struct ParserInternalObjectRecordT* parent;
} ParserObjectRecordT;

struct recWithData
{
    ParserObjectRecordT* recPart;
    struct recordData* pointerToData;
};

struct objWithData
{
    ParserObjectT* objPart;
    struct objData* pointerToData;
};

typedef struct
{
    int line, pos;
    int code;
} ParserErrorT;

// all destructors free memory for object and it's internal structures
void exprDestructor(struct expr* a);
void attrDestructor(ParserObjectAttrT* a);
void objDestructor(ParserObjectT* a);
void objRecordDestructor(ParserObjectRecordT* a);
void parseErrorDestructor(ParserErrorT* a);

// utilities
int isInCharSet(char ch, char* desc); // return positive integer if in,
                                                  // negative if error, zero if not in
ParserErrorT* getLastError(); // (!) copy global structure to result,
int wasError();
const char* errorMessageByCode(int errCode);
struct objWithData findObject(const char* name, struct recWithData rec, int goUp);
int getSeqLen(struct objWithData info);

// important functions
void initialize(const char* buf);
void finalize();
ParserObjectRecordT* parseObjRecord(); // (!)

void ParserValidateFormatDescription(
    const char* data,
    ParserObjectRecordT** tree,
    ParserErrorT** error
); // (!)

const char* ParserGetErrorMessageByCode(int errCode);

// mainly for cats web-server
ParserErrorT* parserValidate(const char* buf);
int getErrCode(ParserErrorT* a);
size_t getErrLine(ParserErrorT* a);
size_t getErrPos(ParserErrorT* a);

struct recWithData mallocRecord(struct recWithData info, int isRoot);
int64_t evaluate(struct expr* e, struct objWithData info);

int64_t getFloatDig(struct objWithData info);
int64_t getIntValue(struct objWithData info);
long double getFloatValue(struct objWithData info);
char* getStrValue(struct objWithData info); // just a pointer, no copy

void setIntValue(struct objWithData info, const int64_t value);
void setFloatValue(struct objWithData info, const long double value);
void setStrValue(struct objWithData info, const char* value); // copy value

struct recWithData byIndex(struct objWithData info, int64_t index);
struct objWithData byName(struct recWithData info, const char* name);

void autoGenRecord(struct recWithData info);
void autoGenSeq(struct objWithData info);
void autoGenInt(struct objWithData info);
void autoGenFloat(struct objWithData info);
void autoGenStr(struct objWithData info);

void destroyRecData(struct recWithData info);
void destroySeqData(struct objWithData info);

// user should destroy stuctures, returned by procedures marked with (!),
// user should also destroy all data by destroy...Data procedures

#endif // __PARSER_H__
