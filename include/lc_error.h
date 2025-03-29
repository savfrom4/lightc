#ifndef LC_ERROR_H
#define LC_ERROR_H
#include "lc_types.h"

typedef enum
{
    LC_E_UNKNOWN = 0,
    LC_E_OTHER = -1,
    LC_E_INVALID_ARGUMENT = -2,
    LC_E_ALLOC_FAILED = -3,
    LC_E_VM_STACK_OVERFLOW = -4,
    LC_E_VM_EARLY_EOF = -5,
    LC_E_PARSE_ERROR = -6,
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