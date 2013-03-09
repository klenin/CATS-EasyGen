%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Allocator.h"
#include "ValidatorTokenizer.h"

extern int yylex();
extern void yyrestart(FILE *new_file);

const char* g_ValidatorTokenizerTokenType2Str[VTT_COUNT] =
{
    "newline",
    "integer",
    "float",
    "string",
    "EOF"
};

// Internal function, defined to avoid linking with flex library.
// Called by tokenizer when EOF is discovered.
int yywrap(void)
{
    return 1;
}

static int yyeof = 0;
static int yyrewind = 0;

int32_t yyline = 0;
int32_t yypos = 0;
ValidatorTokenizerTokenT *yycurrenttoken = NULL;

int32_t ValidatorTokenizerGetCurrentLine()
{
    return yyline + 1;
}

int32_t ValidatorTokenizerGetCurrentPosition()
{
    return yypos - (yypos > 0 ? yyleng: 0) + 1;
}

uint32_t yytoklen()
{
    return yyleng;
}

char* yytoktext()
{
    return yytext;
}

char* yytoktextcopy()
{
    char* token = AllocateBuffer(yyleng + 1);
    strcpy(token, yytext);
    return token;
}

void ValidatorTokenizerRewind()
{
    yyrewind = 1;
}

ValidatorTokenizerTokenT *ValidatorTokenizerNextToken()
{
    ValidatorTokenizerTokenT *token;

    if (yyeof)
    {
        return NULL;
    }

    if (yyrewind)
    {
        yyrewind = 0;
        return yycurrenttoken;
    }

    token = AllocateBuffer(sizeof(ValidatorTokenizerTokenT));
    token->type = yylex();
    yyeof = token->type == VTT_EOF;
    token->text = yytoktextcopy();
    token->length = yytoklen();
    token->line = ValidatorTokenizerGetCurrentLine();
    token->pos = ValidatorTokenizerGetCurrentPosition();
    yycurrenttoken = token;
    return token;
}

void ValidatorTokenizerDestroyToken(ValidatorTokenizerTokenT *token)
{
    if (token != NULL)
    {
        free(token->text);
        free(token);
    }
}

void ValidatorTokenizerSetInput(FILE *handle)
{
    yyrestart(handle);
    yyeof = 0;
}

%}

DIG [0-9]
SGN [-+]
NUM {SGN}?{DIG}+
EXP [Ee]{NUM}
NWL \r\n|\n

%%
[ \t]+ {
    yypos += yyleng;
}

{NWL} {
    yyline++;
    yypos = 0;
    return VTT_NEWLINE;
}

{NUM}  {
     yypos += yyleng;
     return VTT_INTEGER;
}

{NUM}((\.{DIG}+)|{EXP}|(\.{DIG}+{EXP})) {
    yypos += yyleng;
    return VTT_FLOAT;
}

[^ \r\n\t]+ {
    yypos += yyleng;
    return VTT_STRING;
}

<<EOF>>	{
    return VTT_EOF;
}
%%
