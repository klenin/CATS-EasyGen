#include "Allocator.h"
#include "ParserObjectRecordIterator.h"

static ParserObjectRecordIteratorT *ParserCreateObjectRecordIterator(
    ParserObjectRecordT *record
)
{
    ParserObjectRecordIteratorT *iterator = AllocateBuffer(
        sizeof(ParserObjectRecordIteratorT));
    iterator->record = record;
    iterator->index = -1;
    return iterator;
}

int ParserObjectRecordIteratorIsValid(ParserObjectRecordIteratorT *iterator)
{
    return (iterator != NULL) && (iterator->record != NULL) &&
        (iterator->index >= 0 && iterator->index < iterator->record->n);
}

ParserObjectRecordIteratorT *ParserObjectRecordGetFrontElement(
    ParserObjectRecordT *record
)
{
    ParserObjectRecordIteratorT *it = ParserCreateObjectRecordIterator(record);
    it->index++;
    return it;
}

ParserObjectT *ParserObjectRecordIteratorDereference(
    ParserObjectRecordIteratorT *iterator
)
{
    return ParserObjectRecordIteratorIsValid(iterator) ?
        &iterator->record->seq[iterator->index] : NULL;
}

void ParserObjectRecordIteratorAdvance(ParserObjectRecordIteratorT *iterator)
{
    if (iterator != NULL)
    {
        iterator->index++;
    }
}
