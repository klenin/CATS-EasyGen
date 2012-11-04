#include <stdio.h>

#include "tokenizer.h"

int32_t main()
{
    freopen("formal.input", "r", stdin);

    int code;
    do
    {
        code = yylex();
        printf("%d: '%s' (%d chars)\n", code, yytoktext(), yytoklen());
    } while (code != ttEof);

    fclose(stdin);

    return 0;
}
