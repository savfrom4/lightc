#ifndef LC_ERROR_H
#define LC_ERROR_H
#include "lc_types.h"

typedef enum
{
    LCE_UNKNOWN = 0,
    LCE_OTHER = -1,

    LCE_INVALID_ARGUMENT = -2,
    LCE_ALLOC_FAILED = -3,

    LCE_VM_STACK_OVERFLOW = -4,
    LCE_VM_EARLY_EOF = -5,

    LCE_PARSE_ERROR = -6,
} lc_error;

#define lc_error_return_if(error, result, expression) \
    do                                                \
    {                                                 \
        if ((expression))                             \
        {                                             \
            lc_error_set(error, #expression);         \
            return result;                            \
        }                                             \
    } while (0);

lc_void lc_error_set(lc_error error, const lc_char *context);
lc_error lc_error_kind(lc_void);
const lc_char *lc_error_msg(lc_void);

#endif