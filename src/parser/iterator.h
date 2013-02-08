#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include "parser.h"

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

#endif // __ITERATOR_H__
