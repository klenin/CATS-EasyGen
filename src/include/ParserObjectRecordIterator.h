#ifndef __PARSER_OBJECT_RECORD_ITERATOR_H__
#define __PARSER_OBJECT_RECORD_ITERATOR_H__

#include "Parser.h"

typedef struct
{
    ParserObjectRecordT *record;
    int32_t index;
} ParserObjectRecordIteratorT;

int ParserObjectRecordIteratorIsValid(ParserObjectRecordIteratorT *iterator);
ParserObjectRecordIteratorT *ParserObjectRecordGetFrontElement(
    ParserObjectRecordT *record
);
ParserObjectT *ParserObjectRecordIteratorDereference(
    ParserObjectRecordIteratorT *iterator
);
void ParserObjectRecordIteratorAdvance(ParserObjectRecordIteratorT *iterator);

#endif // __PARSER_OBJECT_RECORD_ITERATOR_H__
