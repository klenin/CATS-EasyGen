//#define PARSER_DEBUG_MODE
#ifdef PARSER_DEBUG_MODE

#include <stdio.h>
#include <string.h>

#include "ParserDebug.h"

#define tmpBufSize 10240

static char* buf1;

void debugPrintExpr(struct expr* ex)
{
    if (!ex->opCode) {
        if (ex->varName) printf("%s",ex->varName);
        else printf("%I64d",ex->intConst);
    } else {
        printf("(");
        debugPrintExpr(ex->op1);
        printf(") %c (",ex->opCode);
        debugPrintExpr(ex->op2);
        printf(")");
    }
}

static void debugReadFile(char* name)
{
    char tmp[tmpBufSize];
    size_t bufSize = 0;
    FILE* input = fopen(name, "r");
    buf1 = 0;
    while (fgets(tmp, tmpBufSize, input)) {
        size_t tmpLen = strlen(tmp);
        size_t oldSize = bufSize;
        if (!bufSize) bufSize++;
        bufSize += tmpLen;
        buf1 = ReallocateBuffer(buf1, bufSize);
        buf1[oldSize] = 0;
        strcat(buf1, tmp);
    }
    fclose(input);
}

void debugObjKindPrint(ParserObjectKindT objKind)
{
    if (objKind < PARSER_OBJECT_KINDS_COUNT) {
        printf(g_ParserObjectKind2Str[objKind]);
    } else {
        printf("BAD OBJECT!");
    }
}

void debugCharSetPrint(char* s)
{
    int i;
    for (i = 0; i < 256; i++) if (isInCharSet(i, s)) printf("%d ",i);
}

void debugPrintobjRecord(ParserObjectRecordT* a, int indent)
{
    int i,j,k;
    ParserObjectT* co;

    /*if (lastError) {
        if (a) printf("Error, but objRecord is not destroyed\n");
        printf("Error: %s. Line %d, pos %d\n",
            ParserGetErrorMessageByCode(lastError->code),
            lastError->line, lastError->pos);
        return;
    }*/

    for (k = 0; k < a->n; k++) {
        for (j = 0; j < indent; j++) printf(" ");
        co = &(a->seq[k]);
        debugObjKindPrint(co->objKind); printf(" ");
        for (i = 0; i < PARSER_OBJECT_ATTRIBUTE_KINDS_COUNT; i++) {
            if (co->attrList[i].attrName) {
                if (i) printf(", ");
                printf("%s=",co->attrList[i].attrName);
                if (co->attrList[i].strVal) {
                    if (!strncmp(co->attrList[i].attrName,"chars",5))
                        debugCharSetPrint(co->attrList[i].strVal);
                    else printf("%s",co->attrList[i].strVal);
                }
                else if (co->attrList[i].exVal2) {
                    printf("[");
                    debugPrintExpr(co->attrList[i].exVal1);
                    printf(",");
                    debugPrintExpr(co->attrList[i].exVal2);
                    printf("]");
                } else if (co->attrList[i].exVal1)
                    debugPrintExpr(co->attrList[i].exVal1);
                else printf("!!!!!!!");
            }
        }
        printf("\n");
        if (co->rec) {
            debugPrintobjRecord(co->rec, indent + 3);
            for (j = 0; j < indent; j++) printf(" ");
            printf("end\n");
        }
    }
}

int main()
{
    size_t i;
    ParserObjectT* co;
    ParserObjectRecordT* os;

    debugReadFile("format.txt");
    initialize(buf1);

    //Test buffer
    /*for (i = 0; i < bufSize; i++) putc(buf[i],stdout);
    printf("\n\n");*/

    //Tokenizer
    /*printf("Tokens:\n--------------------\n");
    while (1) {
        ct = curToken;
        if (!ct) break;
        printf("type:%d\t value:%d\t line:%d\t pos:%d\t str:\"%s\" \n",
            ct->type,ct->value,ct->line,ct->pos,ct->str);
        moveToNextToken();
    }
    printf("\n\n");
    finalize();*/

    //getNextObject
    //initialize("format.txt");

    os = parseObjRecord();
    debugPrintobjRecord(os,0);

    //printf("\n\n");
    finalize();
    free(buf1);
}

#endif // PARSER_DEBUG_MODE
