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

int32_t yyline = 0;
int32_t yypos = 0;

int32_t yycurline()
{
    return yyline;
}

int32_t yycurpos()
{
    return yypos;
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

ValidatorTokenizerTokenT *ValidatorTokenizerNextToken()
{
    ValidatorTokenizerTokenT *token;

    if (yyeof)
    {
        return NULL;
    }

    token = AllocateBuffer(sizeof(ValidatorTokenizerTokenT));
    token->type = yylex();
    yyeof = token->type == VTT_EOF;
    token->text = yytoktextcopy();
    token->length = yytoklen();
    token->line = yycurline();
    token->pos = yycurpos();
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

%%
[ \t]+              {
                        yypos += yyleng;
                    }

\r|\n|\r\n          {
                        yyline++;
                        yypos = 0;
                        return VTT_NEWLINE;
                    }

-?[0-9]+\.[0-9]+    {
                        yypos += yyleng;
                        return VTT_FLOAT;
                    }

-?[0-9]+            {
                        yypos += yyleng;
                        return VTT_INTEGER;
                    }

[^ \n\t]+           {
                        yypos += yyleng;
                        return VTT_STRING;
                    }

<<EOF>>             {
                        return VTT_EOF;
                    }
%%
