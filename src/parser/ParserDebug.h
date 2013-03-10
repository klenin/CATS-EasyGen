#ifndef __PARSER_DEBUG_H__
#define __PARSER_DEBUG_H__

#ifdef PARSER_DEBUG_MODE

#include "Parser.h"

void debugPrintExpr(struct expr* ex);
void debugObjKindPrint(ParserObjectKindT objKind);
void debugCharSetPrint(char* s);
void debugPrintobjRecord(ParserObjectRecordT* a, int indent);

#endif // PARSER_DEBUG_MODE

#endif // __PARSER_DEBUG_H__
