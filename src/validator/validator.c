#include <stdio.h>

#include "tokenizer.h"

int32_t main()
{
    int32_t code;

    freopen("formal.input", "r", stdin);

    do
    {
        code = yylex();
        printf("%d: '%s' (%d chars)\n", code, yytoktext(), yytoklen());
    } while (code != ttEof);

    fclose(stdin);

    return 0;
}
