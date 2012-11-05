%{
#include "tokenizer.h"

// Internal function, defined to avoid linking with flex library.
// Called by tokenizer when EOF is discovered.
int yywrap()
{
    return 1;
}

size_t yyline = 0;
size_t yypos = 0;

int32_t yycurline()
{
    return yyline;
}

int32_t yycurpos()
{
    return yypos;
}

size_t yytoklen()
{
    return yyleng;
}

char* yytoktext()
{
    return yytext;
}

char* yytoktextcopy()
{
    char* token = malloc(yyleng);
    strncpy(token, yytext, yyleng);
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
