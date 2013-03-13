//TODO:
//    more precise line and pos in error generation

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Allocator.h"
#include "Exceptions.h"
#include "Numerics.h"
#include "Parser.h"
#include "ParserError.h"
#include "ParserObjectRecordIterator.h"
#include "Random.h"

#define charNumber 256

#define priorityNumber 3

enum tokenType {ttDelim, ttObject, ttAttrName, ttIdentifier,
    ttInteger, ttCharSet};
/* Delimeters list is useless while all of it have length == 1.
enum delimeters {dlSemi, dlComma, dlEq, dlOpenR, dlCloseR, dlOpenSq,
    dlCLoseSq, dlPlus, dlMinus, dlMul, dlDiv, dlDegree};*/

struct token
{
    int type;
    int64_t value;
    size_t line, pos;
    char* str;
};

static const char* buf;
static size_t bufSize, bufPos;
static int line, pos;
static struct token* curToken;
static ParserErrorT* lastError;

static void
    (*g_ObjectGenerators[PARSER_OBJECT_KINDS_COUNT])(ParserObjectWithDataT) =
{
    NULL, // Newline
    NULL, // Softline
    autoGenInt,
    autoGenFloat,
    autoGenStr,
    autoGenSeq,
    NULL // End
};

static void genParseError(int errCode);

//------------------------------ work with memory procedures-------------------
void parseErrorDestructor(ParserErrorT* a)
{
    free(a);
}

static void tokenDestructor(struct token *a)
{
    if (!a) return;
    free(a->str);
    free(a);
}

void exprDestructor(struct expr* a)
{
    if (!a) return;
    if (a->varName) free(a->varName);
    if (a->op1) exprDestructor(a->op1);
    if (a->op2) exprDestructor(a->op2);
    free(a);
}

static void attrDestructor1(ParserObjectAttrT* a)
{
    if (!a) return;
    free(a->attrName); free(a->strVal); free(a->charSetStr);
    if (a->exVal1) {
        exprDestructor(a->exVal1);
        if (a->exVal2) exprDestructor(a->exVal2);
    }
}

void attrDestructor(ParserObjectAttrT* a)
{
    attrDestructor1(a); free(a);
}

void attrListDestructor(ParserObjectAttrT* attrList)
{
    int i;
    if (!attrList) return;
    for (i = 0; i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT; i++)
    {
        attrDestructor1(attrList + i);
    }
    free(attrList);
}

static void objDestructor1(ParserObjectT* a)
{
    if (!a) return;
    attrListDestructor(a->attrList);
    ParserDestroyObjectRecord(a->rec);
}

void objDestructor(ParserObjectT* a)
{
    objDestructor1(a);
    free(a);
}

void ParserDestroyObjectRecord(ParserObjectRecordT* a)
{
    int i;
    if (!a) return;
    for (i = 0; i < a->n; i++) objDestructor1(a->seq + i);
    free(a->seq);
    free(a);
}

static void allocWithCopyStr(char** a, char* b)
{
    if (!b) {*a = 0; return;}
    *a = AllocateBuffer(strlen(b) + 1);
    strcpy(*a, b);
}

static void allocWithCopyExpr(struct expr** a, struct expr* b)
{
    if (!b) {*a = 0; return;}
    *a = AllocateBuffer(sizeof(struct expr));
    (*a)->intConst = b->intConst;
    (*a)->opCode = b->opCode;
    allocWithCopyStr(&((*a)->varName), b->varName);
    allocWithCopyExpr(&((*a)->op1), b->op1);
    allocWithCopyExpr(&((*a)->op2), b->op2);
}

static void copyObjToObj(ParserObjectT* a, ParserObjectT* b);

static void allocWithCopyObjRecord(ParserObjectRecordT** a, ParserObjectRecordT* b)
{
    int i;
    if (!b) {*a = 0; return;}
    *a = AllocateBuffer(sizeof(ParserObjectRecordT));
    (*a)->n = b->n;
    (*a)->parent = b->parent;
    (*a)->seq = AllocateArray(b->n, sizeof(ParserObjectT));
    for (i = 0; i < b->n; i++) {
        copyObjToObj((*a)->seq + i, b->seq + i);
        (*a)->seq[i].parent = *a;
        if ((*a)->seq[i].objKind == PARSER_OBJECT_KIND_SEQUENCE) (*a)->seq[i].rec->parent = *a;
    }
}

static void copyAttrToAttr(ParserObjectAttrT* a, ParserObjectAttrT* b);

static void allocWithCopyAttrList(ParserObjectAttrT** a, ParserObjectAttrT* b, int n)
{
    int i;
    if (!b) {*a = 0; return;}
    *a = AllocateArray(n, sizeof(ParserObjectAttrT));
    for (i = 0; i < n; i++) copyAttrToAttr((*a) + i, b + i);
}

static void copyAttrToAttr(ParserObjectAttrT* a, ParserObjectAttrT* b)
{
    a->charNum = b->charNum;
    memcpy(&(a->valid), &(b->valid), charNumber);
    allocWithCopyStr(&(a->attrName), b->attrName);
    allocWithCopyStr(&(a->strVal), b->strVal);
    allocWithCopyStr(&(a->charSetStr), b->charSetStr);
    allocWithCopyExpr(&(a->exVal1), b->exVal1);
    allocWithCopyExpr(&(a->exVal2), b->exVal2);
}

static void copyObjToObj(ParserObjectT* a, ParserObjectT* b)
{
    a->objKind = b->objKind;
    a->parent = b->parent;
    allocWithCopyObjRecord(&(a->rec), b->rec);
    allocWithCopyAttrList(&(a->attrList), b->attrList,
        PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT);
}

static void copyErrToErr(ParserErrorT* a, ParserErrorT* b)
{
    a->code = b->code; a->line = b->line; a->pos = b->pos;
}

////-------------------------------------utilities-------------------------------

int getAttrCount(ParserObjectKindT objKind)
{
    int i, res = 0;

    if (objKind < PARSER_OBJECT_KINDS_COUNT) {
        for (i = 0; i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT; i++) {
            if (g_ParserObjectAttrKindIsValidForObjectKind[objKind][i]) res++;
        }
    }

    return res;
}

int isInCharSet(char ch, char* desc)
{
    int inv,i,j,lastChar,curChar,range,t,screen;
    size_t n = strlen(desc);
    if (n < 3) return -1;
    inv = (desc[0] == '!');
    if (inv) i = 1; else i = 0;
    if (desc[i] != '{' || desc[n-1] != '}') return -1;
    i++;
    lastChar = charNumber; range = 0;
    while (i < (int)n-1) {
        if (desc[i] == '\\') {
            screen = 1;
            if (i >= (int)n-2) return -1;
            if (desc[i+1] == '-' || desc[i+1] == '}' || desc[i+1] == '\\') {
                curChar = desc[i+1]; i += 2;
            } else if (isdigit((unsigned char)desc[i+1])) {
                j = 1; curChar = 0;
                while (i+j < (int)n-1 && isdigit((unsigned char)desc[i+j]) &&
                    curChar*10+desc[i+j]-'0' < charNumber) {
                        curChar = curChar*10 + desc[i+j] - '0';
                        j++;
                }
                i += j;
            } else return -1;
        } else {curChar = desc[i]; i++; screen = 0;}
        if (curChar == '-' && !screen) {
            if (range != 0 || lastChar == charNumber) return -1;
            range = 1;
        } else {
            if (range == 1) {
                if (lastChar > curChar) {
                    t = lastChar; lastChar = curChar; curChar = t;
                }
                for (j = lastChar; j <= curChar; j++)
                    if ((int)ch == j) return 1-inv;
                range = 2;
            } else {
                if (range != 2 && lastChar == (int)ch) return 1-inv;
                lastChar = curChar;
                range = 0;
            }
        }
    }
    if (range == 1) return -1;
    if (range != 2 && lastChar == (int)ch) return 1-inv;
    return inv;
}

const char* ParserGetErrorMessageByCode(int errCode)
{
    if (errCode >= E_ERROR_RANGE_START && errCode < E_NUMBER_OF_ERROR_TYPES) {
        int i;
        for (i = E_ERROR_RANGE_START; i < E_NUMBER_OF_ERROR_TYPES; ++i) {
            if (errorMessages[i].code == errCode) {
                return errorMessages[i].message;
            }
        }
    }
    return errorMessages[E_UNKNOWN_ERROR].message;
}

ParserErrorT* ParserGetLastError()
{
    ParserErrorT* res;
    if (!lastError) return 0;
    res = AllocateBuffer(sizeof(ParserErrorT));
    copyErrToErr(res, lastError);
    return res;
}

int ParserIsErrorRaised()
{
    return lastError != 0;
}

static void genParseError(int errCode)
{
    if (!lastError) {
        lastError = AllocateBuffer(sizeof(ParserErrorT));
        lastError->code = errCode; lastError->line = line; lastError->pos = pos;
    }
}

ParserObjectWithDataT ParserFindObject(const char* name, ParserObjectRecordWithDataT rec, int goUp)
{
    int i;
    ParserObjectWithDataT res;
    ParserObjectT* object;
    char* attrValue;

    while (rec.recPart) {
        for (i = 0; i < rec.recPart->n; i++) {
            object = &(rec.recPart->seq[i]);
            if (g_ParserObjectAttrKindIsValidForObjectKind
                    [object->objKind][PARSER_OBJECT_ATTR_NAME])
            {
                attrValue = object->attrList[PARSER_OBJECT_ATTR_NAME].strVal;
                if (!strcmp(name, attrValue)) {
                    res.objPart = object;
                    if (rec.pointerToData) {
                        res.pointerToData = &(rec.pointerToData->data[i]);
                    }
                    return res;
                }
            }
        }
        if (!goUp) break;
        rec.recPart = rec.recPart->parent;
        if (rec.pointerToData) {
            rec.pointerToData = rec.pointerToData->parentData->parentData;
        }
    }
    res.objPart = 0; res.pointerToData = 0;
    return res;
}

//--------------------------------parse procedures-----------------------------
static int moveToNextChar()
{
    bufPos++;
    if (bufPos >= bufSize) return 1;
    if (buf[bufPos] == 10) {line++; pos = 0;} else pos++;
    return 0;
}

static int getDelimVal(char ch)
{
    switch (ch) {
        case ';': case ',': case '=': case '(': case ')':
        case '[': case ']': case '+': case '-': case '*':
        case '/': case '^':
            return ch;
        default: return -1;
    }
}

static char* getSubString(const char* string, size_t left, size_t right)
{
    char* substr = NULL;
    if (right >= left) {
        size_t len = right - left;
        substr = AllocateBuffer(len + 2);
        substr[0] = 0;
        strncat(substr, string + left, len + 1);
    }
    return substr;
}

static struct token* newToken(size_t tokLine, size_t tokPos, size_t i, size_t j)
{
    struct token *res = AllocateBuffer(sizeof(struct token));
    res->line = tokLine;
    res->pos = tokPos;
    res->str = getSubString(buf, i, j - 1);
    return res;
}

static void moveToNextToken()
{
    int state = 0, ch, fin, i, ch1;
    size_t line1 = line, pos1 = pos, oldPos = bufPos, sc;

    if (curToken) tokenDestructor(curToken);
    curToken = 0;
    if (bufPos >= bufSize) return;
    while (1) {
        ch = buf[bufPos];
        switch(state) {
            case 0:
                if (0 <= ch && ch <= ' ') {
                    if (moveToNextChar()) return;
                    oldPos = bufPos;
                } else if (isalpha(ch) || ch == '_') {
                    moveToNextChar(); state = 1;
                }
                else if (isdigit(ch)) state = 3;
                else if (ch == '{') state = 4;
                else if (ch == '!') state = 5;
                else state = 2;
                break;

            case 1:
                fin = 0;
                if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                    if (moveToNextChar()) fin = 1;
                } else fin = 1;
                if (fin) {
                    curToken = newToken(line1, pos1, oldPos, bufPos);
                    for (i = 0; i < PARSER_OBJECT_KINDS_COUNT; i++)
                        if (bufPos-oldPos == strlen(g_ParserObjectKind2Str[i]) &&
                            !strncmp(buf+oldPos, g_ParserObjectKind2Str[i], bufPos-oldPos)) break;
                    if (i < PARSER_OBJECT_KINDS_COUNT) {
                        curToken->type = ttObject; curToken->value = i;
                    } else {
                        for (i = 0; i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT; i++)
                            if (bufPos-oldPos == strlen(g_ParserObjectAttrKind2Str[i]) &&
                                !strncmp(buf+oldPos, g_ParserObjectAttrKind2Str[i], bufPos-oldPos)) break;
                        if (i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT) {
                            curToken->type = ttAttrName; curToken->value = i;
                        } else {
                            curToken->type = ttIdentifier; curToken->value = -1;
                        }
                    }
                    return;
                }
                break;

            case 2:
                i = getDelimVal(ch);
                moveToNextChar();
                if (i != -1) {
                    curToken = newToken(line1, pos1, oldPos, bufPos);
                    curToken->type = ttDelim; curToken->value = i;
                    return;
                } else {
                    genParseError(E_UNKNOWN_TOKEN);
                    return;
                }
                break;
            case 3:
                fin = 0;
                if (isdigit(ch)) {
                    if (moveToNextChar()) fin = 1;
                } else fin = 1;
                if (fin) {
                    curToken = newToken(line1,pos1,oldPos,bufPos);
                    curToken->type = ttInteger; curToken->value = 0;
                    if (bufPos - oldPos > LLONG_MAX_LEN) {
                        genParseError(E_TOO_LONG_INTEGER);
                        return;
                    }
                    for (sc = oldPos; sc < bufPos; sc++) {
                        curToken->value = curToken->value*10 + buf[sc] - '0';
                    }
                    return;
                }
                break;
            case 4:
                if (moveToNextChar()) {
                    genParseError(E_RCB_EXPECTED);
                    return;
                }
                if (buf[bufPos] == '\\') state = 6;
                if (buf[bufPos] == '}') {
                    moveToNextChar();
                    curToken = newToken(line1,pos1,oldPos,bufPos);
                    curToken->type = ttCharSet; curToken->value = 0;
                    return;
                }
                break;
            case 5:
                if (moveToNextChar() || buf[bufPos] != '{') {
                    genParseError(E_LCB_EXPECTED);
                    return;
                }
                state = 4;
                break;
            case 6:
                if (moveToNextChar()) {
                    genParseError(E_RCB_EXPECTED);
                    return;
                }
                ch1 = buf[bufPos];
                if (ch1 == '\\' || ch1 == '-' || ch1 == '}' || isdigit(ch1)) {
                    state = 4;
                } else {
                    genParseError(E_UNEXPECTED_CHAR_AFTER_BACKSLASH);
                    return;
                }
                break;
            default:
                genParseError(B_UNKNOWN_STATE);
                return;
        }
    }
}

static int chkCurToken(char ch)
{
    return curToken && curToken->type == ttDelim && curToken->value == ch;
}

static int attrFind(char *name)
{
    int i;
    for (i = 0; i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT; i++)
        if (!strcmp(g_ParserObjectAttrKind2Str[i], name)) return i;
    return -1;
}

static char* parseSingleToken(int tokenType)
{
    char* res = NULL;
    if (curToken && curToken->type == tokenType) {
        res = AllocateBuffer(strlen(curToken->str) + 1);
        strcpy(res, curToken->str);
        moveToNextToken();
    } else {
        genParseError(E_UNEXPECTED_TOKEN);
    }
    return res;
}

static int getOpPriority(char op)
{
    switch (op) {
        case '+': case '-' : return 1;
        case '*': case '/' : return 2;
        case '^': return 3;
    }
    return 0;
}

static struct expr* parseExpression1(int priority, int isFirst,
    ParserObjectRecordT* curSeq)
{
    struct expr *res, *op;
    ParserObjectT* varObj;
    ParserObjectRecordWithDataT tmp;
    char ch;
    if (priority > priorityNumber) {
        switch (curToken->type) {
            case ttIdentifier:
                res = AllocateBuffer(sizeof(struct expr));
                res->op1 = res->op2 = 0; res->opCode = 0;
                res->varName = AllocateBuffer(strlen(curToken->str) + 1);
                strcpy(res->varName, curToken->str);

                tmp.pointerToData = 0; tmp.recPart = curSeq;
                varObj = ParserFindObject(res->varName, tmp, 1).objPart;
                if (!varObj) genParseError(E_UNKNOWN_OBJECT);
                if (!ParserIsErrorRaised() && varObj->objKind != PARSER_OBJECT_KIND_INTEGER) genParseError(E_INTEGER_OBJECT_EXPECTED);
                if (ParserIsErrorRaised()) {exprDestructor(res); return 0;}
                moveToNextToken();
                if (ParserIsErrorRaised()) {exprDestructor(res); return 0;}
                return res;
            case ttInteger:
                res = AllocateBuffer(sizeof(struct expr));
                res->op1 = res->op2 = 0; res->opCode = 0; res->varName = 0;
                res->intConst = curToken->value;
                moveToNextToken();
                if (ParserIsErrorRaised()) {exprDestructor(res); return 0;}
                return res;
            case ttDelim:
                if (curToken->value == '(') {
                    moveToNextToken();
                    res = parseExpression1(1, 1, curSeq);
                    if (res && !chkCurToken(')')) {
                        genParseError(E_RB_EXPECTED);
                        exprDestructor(res);
                        return NULL;
                    }
                    moveToNextToken();
                    return res;
                }
                // don't need "break" here
            default:
                return NULL;
        }
    } else {
        op = parseExpression1(priority + 1, 1, curSeq);

        if (ParserIsErrorRaised()) {exprDestructor(op); return NULL;}
        if (!op) {
            if (priority == getOpPriority('+') && isFirst &&
                (chkCurToken('+') || chkCurToken('-'))) { // unary operations
                    op = AllocateBuffer(sizeof(struct expr));
                    op->op1 = op->op2 = 0; op->opCode = 0; op->varName = 0;
                    op->intConst = 0;
            } else {
                if (priority == getOpPriority('+')) {
                    genParseError(E_OPERAND_EXPECTED);
                }
                return NULL;
            }
        }

        if (chkCurToken('^')) {
            if (priority < getOpPriority('^')) {
                genParseError(B_UNEXPECTED_CAP);
                exprDestructor(op);
                return NULL;
            }
            res = AllocateBuffer(sizeof(struct expr));
            res->op1 = op; res->opCode = '^';
            res->varName = 0; res->intConst = 0;
            moveToNextToken();
            res->op2 = parseExpression1(priority, 0, curSeq);
            if (!res->op2) {exprDestructor(res->op2); return NULL;}
            op = res;
        } else {
            while ((chkCurToken('+') || chkCurToken('-') ||
                chkCurToken('*') || chkCurToken('/')) &&
                getOpPriority((int)curToken->value) == priority) {
                    ch = (char)curToken->value;
                    res = AllocateBuffer(sizeof(struct expr));
                    res->op1 = op; res->opCode = ch;
                    res->varName = 0; res->intConst = 0;
                    moveToNextToken();
                    res->op2 = parseExpression1(priority, 0, curSeq);
                    if (!res->op2) {exprDestructor(res); return NULL;}
                    op = res;
            }
        }
        if (ParserIsErrorRaised()) {exprDestructor(op); return NULL;}
        return op;
    }
}

static struct expr* parseExpression(ParserObjectRecordT* curSeq)
{
    return parseExpression1(1,1,curSeq);
}

static void parseIntRange(struct expr** ex1, struct expr** ex2, ParserObjectRecordT* curSeq)
{
    if (chkCurToken('[')) {
        moveToNextToken();
        *ex1 = parseExpression(curSeq);
        if (!*ex1 || !chkCurToken(',')) {
            exprDestructor(*ex1); *ex1 = *ex2 = 0; return;
        }
        moveToNextToken();
        *ex2 = parseExpression(curSeq);
        if (!*ex2 || !chkCurToken(']')) {
            exprDestructor(*ex1); exprDestructor(*ex2);
            *ex1 = *ex2 = 0; return;
        }
        moveToNextToken();
    } else {
        *ex1 = parseExpression(curSeq); *ex2 = 0;
    }
}

static ParserObjectAttrT* parseAttr(ParserObjectRecordT* curSeq)
{
    ParserObjectAttrT* res;
    int i, j, k;

    if (!curToken || curToken->type != ttAttrName) return 0;
    res = AllocateBuffer(sizeof(ParserObjectAttrT));
    memset(res,0,sizeof(ParserObjectAttrT));
    res->attrName = AllocateBuffer(strlen(curToken->str) + 1);
    strcpy(res->attrName, curToken->str);
    moveToNextToken();
    if (!chkCurToken('=')) {attrDestructor(res); return 0;}
    moveToNextToken();

    i = attrFind(res->attrName);
    if (i < 0) {
        genParseError(E_UNKNOWN_ATTRIBUTE);
        attrDestructor(res);
        return NULL;
    }

    if (!curToken) return NULL;
    switch (g_ParserObjectKind2AttrValueKind[i]) {
        case PARSER_OBJECT_ATTR_VALUE_IDENTIFIER:
            res->strVal = parseSingleToken(ttIdentifier);
            if (!res->strVal) {attrDestructor(res); return NULL;}
            break;
        case PARSER_OBJECT_ATTR_VALUE_INTEGER:
            res->exVal1 = parseExpression(curSeq);
            if (!res->exVal1) {attrDestructor(res); return NULL;}
            break;
        case PARSER_OBJECT_ATTR_VALUE_INTEGER_RANGE:
            parseIntRange(&res->exVal1, &res->exVal2, curSeq);
            if (!res->exVal1) {attrDestructor(res); return NULL;}
            break;
        case PARSER_OBJECT_ATTR_VALUE_CHARSET:
            res->strVal = parseSingleToken(ttCharSet);
            if (!res->strVal) {attrDestructor(res); return NULL;}
            if (isInCharSet(0, res->strVal) < 0) {
                genParseError(E_INVALID_CHARSET);
                attrDestructor(res); return NULL;
            }
            res->charNum = 0;
            memset(res->valid, 0, sizeof(res->valid));
            for (j = 0; j < charNumber; j++) {
                res->valid[j] = isInCharSet(j, res->strVal);
                if (res->valid[j]) res->charNum++;
            }
            res->charSetStr = AllocateBuffer(res->charNum+1);
            res->charSetStr[res->charNum] = 0; k = 0;
            for (j = 0; j < charNumber; j++)
                if (res->valid[j]) res->charSetStr[k++] = j;
            break;
        default:
            genParseError(B_ARRAY_ATTR_TYPE_BY_KIND);
            attrDestructor(res); return NULL;
    }
    if (ParserIsErrorRaised()) {attrDestructor(res); return NULL;}
    return res;
}

static ParserObjectAttrT* parseAttrList(
    ParserObjectKindT objKind,
    ParserObjectRecordT* curSeq
)
{
    //it should eat semicolons too
    ParserObjectAttrT *res = 0;
    ParserObjectAttrT *curAttr;
    int bad = 0, i, n = 0;
    int vis[PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT];

    res = AllocateArray(PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT,
        sizeof(ParserObjectAttrT));
    memset(vis, 0, sizeof(vis));

    while ((curAttr = parseAttr(curSeq))) {
        i = attrFind(curAttr->attrName);
        if (vis[i]) {
            genParseError(E_ATTRIBUTE_ALREADY_DEFINED);
            attrDestructor(curAttr); curAttr = 0;
            bad = 1; break;
        }
        if (!g_ParserObjectAttrKindIsValidForObjectKind[objKind][i]) {
            genParseError(E_INVALID_ATTRIBUTE);
            attrDestructor(curAttr); curAttr = 0;
            bad = 1; break;
        }
        vis[i] = 1; n++;
        copyAttrToAttr(&(res[i]), curAttr);
        attrDestructor(curAttr);

        if (chkCurToken(',')) {
            bad = 1; moveToNextToken();
        } else bad = 0;
    }

    if (ParserIsErrorRaised()) {attrListDestructor(res); return NULL;}
    if (n < getAttrCount(objKind)) bad = 1;
    if (bad) {
        attrListDestructor(res);
        genParseError(E_INVALID_ATTRIBUTE_LIST);
        return NULL;
    }
    if (!curToken || chkCurToken(';')) {
        if (curToken) moveToNextToken();
        return res;
    } else {attrListDestructor(res); return NULL;}
}

static ParserObjectRecordT* parseObjRecord1(ParserObjectRecordT* parent);

static ParserObjectT* parseNextObject(ParserObjectRecordT* curSeq)
{
    ParserObjectKindT objKind;
    ParserObjectT* res;
    ParserObjectAttrT* attrList;
    char* name;
    ParserObjectRecordWithDataT tmp;

    if (!curToken) return NULL;
    if (curToken->type != ttObject) {
        genParseError(E_OBJECT_KIND_EXPECTED);
        return NULL;
    }
    objKind = curToken->value;
    moveToNextToken();

    attrList = parseAttrList(objKind, curSeq);

    if (attrList) {
        name = attrList[PARSER_OBJECT_ATTR_NAME].strVal;
        if (name) { // if name doesn't exist, than it's unnamed objKind
            tmp.pointerToData = 0; tmp.recPart = curSeq;
            if (ParserFindObject(name, tmp, 0).objPart) {
                genParseError(E_DUPLICATE_OBJECT);
                attrListDestructor(attrList);
                return NULL;
            }
        }
    }

    if (attrList ||
        objKind == PARSER_OBJECT_KIND_END ||
        objKind == PARSER_OBJECT_KIND_NEWLINE ||
        objKind == PARSER_OBJECT_KIND_SOFTLINE)
    {
            res = AllocateBuffer(sizeof(ParserObjectT));
            res->attrList = attrList; res->objKind = objKind;
            if (objKind == PARSER_OBJECT_KIND_SEQUENCE) {
                res->rec = parseObjRecord1(curSeq);
                if (!res->rec) {objDestructor(res); return 0;}
            } else res->rec = 0;
            if (ParserIsErrorRaised()) {objDestructor(res); return 0;}
            //res->parent = curSeq;  <- curSeq will reallocate anyway
            return res;
    }

    genParseError(E_INVALID_ATTRIBUTE_LIST);
    return NULL;
}

static ParserObjectRecordT* parseObjRecord1(ParserObjectRecordT* parent)
{
    ParserObjectT* curObj;
    ParserObjectRecordT *res, *tmp;
    int wasEnd = 0;
    int i;
    res = AllocateBuffer(sizeof(ParserObjectRecordT));
    res->n = 0; res->seq = 0; res->parent = parent;

    while ((curObj = parseNextObject(res))) {
        if (curObj->objKind == PARSER_OBJECT_KIND_END) {
            if (!parent) {
                genParseError(E_UNEXPECTED_END);
            } else {
                objDestructor(curObj);
                wasEnd = 1;
            }
            break;
        }
        res->n++;
        tmp = AllocateBuffer(sizeof(ParserObjectRecordT));
        tmp->seq = AllocateArray(res->n, sizeof(ParserObjectT));
        for (i = 0; i < res->n - 1; i++) {
            copyObjToObj(tmp->seq + i, res->seq + i);
            tmp->seq[i].parent = tmp;
            if (tmp->seq[i].objKind == PARSER_OBJECT_KIND_SEQUENCE) tmp->seq[i].rec->parent = tmp;
        }
        tmp->parent = res->parent; tmp->n = res->n;
        res->n--; ParserDestroyObjectRecord(res);
        copyObjToObj(tmp->seq + tmp->n - 1, curObj);
        tmp->seq[tmp->n - 1].parent = tmp;
        if (tmp->seq[tmp->n - 1].objKind == PARSER_OBJECT_KIND_SEQUENCE)
            tmp->seq[tmp->n - 1].rec->parent = tmp;
        res = tmp;
        objDestructor(curObj);
    }

    if (!wasEnd && parent) {
        genParseError(E_END_EXPECTED);
    }

    if (ParserIsErrorRaised()) {ParserDestroyObjectRecord(res); return 0;}
    return res;
}

ParserObjectRecordT* parseObjRecord()
{
    return parseObjRecord1(0);
}

//-----------------------------data processing procedures----------------------
ParserObjectRecordWithDataT* ParserAllocateObjectRecordWithData(
    ParserObjectRecordWithDataT *info,
    int isRoot
)
{
    int i;
    if (!info) return NULL;
    if (!info->recPart->n) {info->pointerToData = 0; return info;}
    if (isRoot) {
        info->pointerToData = AllocateBuffer(sizeof(ParserObjectRecordDataT));
        info->pointerToData->parentData = 0;
    }
    info->pointerToData->data =
        AllocateArray(info->recPart->n, sizeof(ParserObjectDataT));
    for (i = 0; i < info->recPart->n; i++) {
        info->pointerToData->data[i].value = 0;
        info->pointerToData->data[i].parentData = info->pointerToData;
    }
    return info;
}

static void ParserEvaluateGenericRange(
    ParserObjectWithDataT info,
    ParserObjectAttributeKindT attrKind,
    int64_t* l,
    int64_t* r
)
{
    ParserObjectAttrT attr = info.objPart->attrList[attrKind];
    *l = evaluate(attr.exVal1, info);
    *r = (attr.exVal2 != NULL) ? evaluate(attr.exVal2, info): *l;
}

void ParserEvaluateIntRange(ParserObjectWithDataT info, int64_t* l, int64_t* r)
{
    ParserEvaluateGenericRange(info, PARSER_OBJECT_ATTR_RANGE, l, r);
}

void ParserEvaluateLenRange(ParserObjectWithDataT info, int64_t* l, int64_t* r)
{
    ParserEvaluateGenericRange(info, PARSER_OBJECT_ATTR_LENGTH_RANGE, l, r);
}

static int isObjectValid(
    ParserObjectWithDataT* info,
    ParserObjectKindT expectedKind
)
{
    if (info->objPart->objKind != expectedKind) {
        genParseError(E_OBJECT_KIND_MISMATCH_GET);
        return 0;
    }
    if (!info->pointerToData->value) {
        genParseError(E_UNINITIALIZED_OBJECT);
        return 0;
    }
    return !0;
}

int64_t ParserGetIntegerValue(ParserObjectWithDataT info)
{
    if (isObjectValid(&info, PARSER_OBJECT_KIND_INTEGER)) {
        return *((int64_t*)info.pointerToData->value);
    }
    return 0;
}

long double ParserGetFloatValue(ParserObjectWithDataT info)
{
    if (isObjectValid(&info, PARSER_OBJECT_KIND_FLOAT)) {
        return *((long double*)info.pointerToData->value);
    }
    return 0;
}

char* ParserGetStringValue(ParserObjectWithDataT info)
{
    if (isObjectValid(&info, PARSER_OBJECT_KIND_STRING)) {
        return (char*)(info.pointerToData->value);
    }
    return NULL;
}

void ParserSetIntegerValue(ParserObjectWithDataT info, const int64_t value)
{
    if (info.objPart->objKind != PARSER_OBJECT_KIND_INTEGER) {
        genParseError(E_ASSIGN_NON_INT);
        return;
    }
    if (!info.pointerToData->value) {
        int64_t l, r;
        ParserEvaluateIntRange(info, &l, &r);
        if (value < l || value > r) {
            genParseError(E_OUT_OF_RANGE);
        } else {
            info.pointerToData->value = AllocateBuffer(sizeof(value));
            memcpy(info.pointerToData->value, &value, sizeof(value));
        }
    } else genParseError(E_VALUE_ALREADY_DEFINED);
}

int64_t ParserGetFloatDigits(ParserObjectWithDataT info)
{
    return evaluate(
        info.objPart->attrList[PARSER_OBJECT_ATTR_DIGITS].exVal1, info);
}

void ParserSetFloatValue(ParserObjectWithDataT info, const long double value)
{
    if (info.objPart->objKind != PARSER_OBJECT_KIND_FLOAT) {
        genParseError(E_ASSIGN_NON_FLOAT);
        return;
    }
    if (!info.pointerToData->value) {
        int64_t l, r, d;
        ParserEvaluateIntRange(info, &l, &r); // it just takes "range" attribute
        if (value < l || value > r) {
            genParseError(E_OUT_OF_RANGE);
            return;
        }
        d = ParserGetFloatDigits(info);
        if (d < 0 || d > LDBL_DIG) {
            genParseError(E_INVALID_FRACTIONAL_PART);
            return;
        }
        info.pointerToData->value = AllocateBuffer(sizeof(value));
        memcpy(info.pointerToData->value, &value, sizeof(value));
    } else genParseError(E_VALUE_ALREADY_DEFINED);
}

void ParserSetStringValue(ParserObjectWithDataT info, const char* value)
{
    int32_t i, n;
    int64_t l, r;
    if (info.objPart->objKind != PARSER_OBJECT_KIND_STRING) {
        genParseError(E_ASSIGN_NON_STRING);
        return;
    }
    if (!info.pointerToData->value) {
        n = (int32_t)strlen(value);
        ParserEvaluateLenRange(info, &l, &r);
        if (ParserIsErrorRaised()) return;
        if (n < l || n > r) {
            genParseError(E_INVALID_STRING_LENGTH);
            return;
        }
        for (i = 0; i < n; ++i) {
            int index = value[i];
            if (!info.objPart->attrList[PARSER_OBJECT_ATTR_CHARS].valid[index]) {
                genParseError(E_INVALID_CHARS);
                return;
            }
        }
        info.pointerToData->value = AllocateBuffer(n + 1);
        memcpy(info.pointerToData->value, value, n + 1);
    } else genParseError(E_VALUE_ALREADY_DEFINED);
}

void autoGenInt(ParserObjectWithDataT info)
{
    int64_t l, r, tmp;
    if (info.objPart->objKind != PARSER_OBJECT_KIND_INTEGER) {
        genParseError(E_GENERATE_NON_INT);
        return;
    }
    if (!info.pointerToData->value) {
        ParserEvaluateIntRange(info, &l, &r);
        if (ParserIsErrorRaised()) return;
        tmp = GenerateRandInt(l, r);
        info.pointerToData->value = AllocateBuffer(sizeof(tmp));
        memcpy(info.pointerToData->value, &tmp, sizeof(tmp));
    }
}

void autoGenFloat(ParserObjectWithDataT info)
{
    if (info.objPart->objKind != PARSER_OBJECT_KIND_FLOAT) {
        genParseError(E_GENERATE_NON_FLOAT);
        return;
    }
    if (!info.pointerToData->value) {
        long double rnd;
        int64_t l, r;
        int64_t d = evaluate(
            info.objPart->attrList[PARSER_OBJECT_ATTR_DIGITS].exVal1, info);
        if (ParserIsErrorRaised()) return;
        ParserEvaluateIntRange(info, &l, &r);
        rnd = GenerateRandFloat(l, r, d);
        info.pointerToData->value = AllocateBuffer(sizeof(rnd));
        memcpy(info.pointerToData->value, &rnd, sizeof(rnd));
    }
}

void autoGenStr(ParserObjectWithDataT info)
{
    char* res;
    int64_t i, l, r, rnd;
    if (info.objPart->objKind != PARSER_OBJECT_KIND_STRING) {
        genParseError(E_GENERATE_NON_STRING);
        return;
    }
    if (!info.pointerToData->value) {
        ParserEvaluateIntRange(info, &l, &r);
        if (ParserIsErrorRaised()) return;
        rnd = GenerateRandInt(l, r);
        res = AllocateBuffer((size_t)rnd + 1);
        for (i = 0; i < rnd; ++i) {
            res[i] = info.objPart->attrList[PARSER_OBJECT_ATTR_CHARS].charSetStr
                [GenerateRandInt(0, info.objPart->attrList[PARSER_OBJECT_ATTR_CHARS].charNum - 1)];
        }
        res[rnd] = 0;
        info.pointerToData->value = res;
    }
}

int ParserGetSequenceLength(ParserObjectWithDataT info)
{
    if (info.objPart->objKind != PARSER_OBJECT_KIND_SEQUENCE)
    {
        genParseError(E_NOT_A_SEQUENCE);
    }
    return (int)evaluate(
        info.objPart->attrList[PARSER_OBJECT_ATTR_LENGTH].exVal1, info);
}

void autoGenSeq(ParserObjectWithDataT info)
{
    ParserObjectRecordWithDataT rnd;
    int i;
    int64_t n = evaluate(
        info.objPart->attrList[PARSER_OBJECT_ATTR_LENGTH].exVal1, info);
    if (ParserIsErrorRaised()) return;
    if (info.objPart->objKind != PARSER_OBJECT_KIND_SEQUENCE) {
        genParseError(E_GENERATE_NON_SEQUENCE);
        return;
    }
    ParserGetSequenceElement(info, n);
    for (i = 0; i < n; i++) {
        rnd.pointerToData = &(((ParserObjectRecordDataT*)(info.pointerToData->value))[i]);
        rnd.recPart = info.objPart->rec;
        autoGenRecord(rnd);
    }
}

void autoGenRecord(ParserObjectRecordWithDataT info)
{
    int i;
    ParserObjectKindT objk;
    ParserObjectWithDataT tmp;
    if (!info.pointerToData->data) {
        info = *ParserAllocateObjectRecordWithData(&info, 0);
    }
    for (i = 0; i < info.recPart->n; i++) {
        tmp.objPart = &(info.recPart->seq[i]);
        tmp.pointerToData = &(info.pointerToData->data[i]);
        objk = (info.recPart->seq[i]).objKind;
        if (g_ObjectGenerators[objk] != NULL &&
            (objk == PARSER_OBJECT_KIND_SEQUENCE ||
             tmp.pointerToData->value == NULL))
        {
                g_ObjectGenerators[objk](tmp);
        }
    }
}

int64_t evaluate(struct expr* e, ParserObjectWithDataT info)
{
    int64_t res = 0;
    int64_t i, val1, val2;
    ParserObjectRecordWithDataT rnd;
    ParserObjectWithDataT tmp;
    if (e->op1) {
        val1 = evaluate(e->op1, info); val2 = evaluate(e->op2, info);
        switch (e->opCode) {
            case '+': res = val1 + val2; break;
            case '-': res = val1 - val2; break;
            case '*': res = val1 * val2; break;
            case '/':
                if (val2 == 0) {
                    genParseError(E_ZERO_DIVISION);
                }
                res = val1 / val2; break;
            case '^':
                res = 1;
                for (i = 1; i <= val2; i++) res *= val1;
                break;
            default:
                genParseError(B_UNKNOWN_OPERATION);
        }
    } else if (e->varName) {
        rnd.pointerToData = info.pointerToData->parentData;
        rnd.recPart = info.objPart->parent;
        tmp = ParserFindObject(e->varName, rnd, 1);
        if (!tmp.objPart) genParseError(B_UNKNOWN_IDENTIFIER);
        res = ParserGetIntegerValue(tmp);
    } else res = e->intConst;
    return res;
}

ParserObjectWithDataT ParserFindObjectByName(
    ParserObjectRecordWithDataT info,
    const char* name
)
{
    return ParserFindObject(name, info, 0);
}

ParserObjectRecordWithDataT ParserGetSequenceElement(
    ParserObjectWithDataT info,
    int64_t index
)
{
    ParserObjectDataT* data = info.pointerToData;
    ParserObjectT* a = info.objPart;
    ParserObjectRecordWithDataT res;
    ParserObjectRecordDataT* d;
    int n = ParserGetSequenceLength(info);
    int i;
    res.pointerToData = 0; res.recPart = 0;
    if (ParserIsErrorRaised()) return res;
    if (index < 1 || index > n) {
        genParseError(E_INVALID_INDEX);
        return res;
    }
    if (!data->value) {
        data->value = AllocateArray(n, sizeof(ParserObjectRecordDataT));
        d = (ParserObjectRecordDataT*)(data->value);
        for (i = 0; i < n; i++) {
            d[i].data = 0;
            d[i].parentData = data;
        }
    }
    res.recPart = a->rec;
    //take 1-indexed value
    res.pointerToData = &(((ParserObjectRecordDataT*)(data->value))[index-1]);
    if (!res.pointerToData->data) res = *ParserAllocateObjectRecordWithData(&res, 0);
    return res;
}

void destroySeqData(ParserObjectWithDataT info)
{
    ParserObjectRecordWithDataT rnd;
    int i, n;
    if (info.objPart->objKind != PARSER_OBJECT_KIND_SEQUENCE) {
        genParseError(E_DESTROY_NON_SEQUENCE);
        return;
    }
    n = (int)evaluate(
        info.objPart->attrList[PARSER_OBJECT_ATTR_LENGTH].exVal1, info);
    if (ParserIsErrorRaised()) return;
    if (!info.pointerToData || !info.pointerToData->value) return;
    for (i = 0; i < n; i++) {
        rnd.pointerToData = &(((ParserObjectRecordDataT*)(info.pointerToData->value))[i]);
        rnd.recPart = info.objPart->rec;
        ParserDestroyObjectRecordWithData(&rnd);
    }
}

void ParserDestroyObjectRecordWithData(ParserObjectRecordWithDataT *info)
{
    ParserObjectWithDataT tmp;
    int i;
    if (info == NULL) return;
    if (info->pointerToData == NULL) return;
    for (i = 0; i < info->recPart->n; i++) {
        tmp.objPart = &(info->recPart->seq[i]);
        tmp.pointerToData = &(info->pointerToData->data[i]);
        if (tmp.pointerToData) {
            switch ((info->recPart->seq[i]).objKind) {
                case PARSER_OBJECT_KIND_INTEGER:
                case PARSER_OBJECT_KIND_FLOAT:
                case PARSER_OBJECT_KIND_STRING:
                    free(tmp.pointerToData->value);
                    break;
                case PARSER_OBJECT_KIND_SEQUENCE:
                    destroySeqData(tmp);
                    break;
                default:
                    genParseError(E_UNEXPECTED_OBJECT_KIND);
            }
        }
    }
}

//-----------------------------initialize & finalize --------------------------
void initialize(const char* buf1)
{
    lastError = 0;
    buf = buf1;
    bufPos = pos = 0; line = 1;
    bufSize = strlen(buf);
    moveToNextToken();
}

void finalize()
{
    if (curToken) tokenDestructor(curToken);
    parseErrorDestructor(lastError);
}

ParserErrorT* ParserValidateFormatDescriptionEx(
    const char* data,
    ParserObjectRecordT** tree
)
{
    E4C_BOOL isUncaught = E4C_FALSE;
    ParserErrorT* error = NULL;

    // Need to create an exception context before catching exceptions.
    e4c_reusing_context(isUncaught, E4C_TRUE)
    {
        try
        {
            initialize(data);
            *tree = parseObjRecord();
        }
        catch (RuntimeException)
        {
            genParseError(E_EXCEPTION);
        }
    }

    if (ParserIsErrorRaised())
    {
        error = AllocateBuffer(sizeof(ParserErrorT));
        copyErrToErr(error, lastError);
    }

    finalize();
    return error;
}

static void ParserPrintDataObject(ParserObjectWithDataT *objData)
{
    static int wasSpaceChar = 1;
    ParserObjectKindT objKind = objData->objPart->objKind;

    switch (objKind)
    {
        case PARSER_OBJECT_KIND_NEWLINE:
        case PARSER_OBJECT_KIND_SOFTLINE:
        {
            printf("\n");
            wasSpaceChar = 1;
            break;
        }
        case PARSER_OBJECT_KIND_INTEGER:
        case PARSER_OBJECT_KIND_FLOAT:
        case PARSER_OBJECT_KIND_STRING:
        {
            if (!wasSpaceChar)
            {
                printf(" ");
            }
            if (objKind == PARSER_OBJECT_KIND_INTEGER)
            {
                printf("%" PRId64, ParserGetIntegerValue(*objData));
            }
            else if (objKind == PARSER_OBJECT_KIND_STRING)
            {
                printf("%s", ParserGetStringValue(*objData));
            }
            else if (objKind == PARSER_OBJECT_KIND_FLOAT)
            {
                printf("%.*" PRLD,
                    (uint32_t)ParserGetFloatDigits(*objData),
                    (PRLDType)ParserGetFloatValue(*objData));
            }
            wasSpaceChar = 0;
            break;
        }
        case PARSER_OBJECT_KIND_SEQUENCE:
        {
            int sequenceLength = ParserGetSequenceLength(*objData);
            int i;
            for (i = 1; i <= sequenceLength; ++i)
            {
                ParserObjectRecordWithDataT recData =
                    ParserGetSequenceElement(*objData, i);
                ParserPrintDataRecord(&recData);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void ParserPrintDataRecord(ParserObjectRecordWithDataT *recData)
{
    ParserObjectT *object = NULL;
    ParserObjectRecordIteratorT *iterator = NULL;

    if (recData == NULL)
    {
        return;
    }

    for (iterator = ParserObjectRecordGetFrontElement(recData->recPart);
                    ParserObjectRecordIteratorIsValid(iterator);
                    ParserObjectRecordIteratorAdvance(iterator))
    {
        ParserObjectWithDataT objData;
        object = ParserObjectRecordIteratorDereference(iterator);
        objData.objPart = object;
        objData.pointerToData = &(recData->pointerToData->data[iterator->index]);
        ParserPrintDataObject(&objData);
    }
    free(iterator);
}

/*******************************************************************************
                            CATS interface functions
*******************************************************************************/

ParserErrorT* ParserValidateFormatDescription(const char *formatDescription)
{
    ParserErrorT* error = NULL;
    ParserObjectRecordT* tree;
    error = ParserValidateFormatDescriptionEx(formatDescription, &tree);
    ParserDestroyObjectRecord(tree);
    return error;
}

const char *ParserGetErrorMessage(ParserErrorT *error)
{
    return ParserGetErrorMessageByCode(error->code);
}

int ParserGetErrorLine(ParserErrorT *error)
{
    return error->line;
}

int ParserGetErrorPos(ParserErrorT *error)
{
    return error->pos;
}
