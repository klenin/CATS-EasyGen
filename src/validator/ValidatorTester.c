#include <stdio.h>

#include "Validator.h"

int32_t main()
{
    ValidatorErrorT *error = ValidatorValidate("data", "formal.input");
    if (error != NULL)
    {
        printf("%d:%d %s\n", error->line, error->pos, error->message);
    }

    return 0;
}
