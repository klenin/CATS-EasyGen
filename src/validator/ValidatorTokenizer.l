%{
#include <stdlib.h>
#include <string.h>

#include "Allocator.h"
#include "ValidatorTokenizer.h"

extern int yylex();

// Internal function, defined to avoid linking with flex library.
// Called by tokenizer when EOF is discovered.
int yywrap(void)
{
    return 1;
}

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
    char* token = AllocateBuffer(yyleng);
    strncpy(token, yytext, yyleng);
    return token;
}

Token *yynexttoken()
{
    Token *token = AllocateBuffer(sizeof(Token));
    token->type = yylex();
    token->text = yytoktextcopy();
    token->length = yytoklen();
    token->line = yycurline();
    token->pos = yycurpos();
    return token;
}

%}

%%
[ \t]+              {
                        yypos += yyleng;
                    }

\r|\n|\r\n          {
                        yyline++;
                        yypos = 0;
                        return ttNewLine;
                    }

-?[0-9]+\.[0-9]+    {
                        yypos += yyleng;
                        return ttFloat;
                    }

-?[0-9]+            {
                        yypos += yyleng;
                        return ttInteger;
                    }

[^ \n\t]+           {
                        yypos += yyleng;
                        return ttString;
                    }

<<EOF>>             {
                        return ttEof;
                    }
%%
