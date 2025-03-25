#ifndef LC_ERROR_H
#define LC_ERROR_H
#include "lc_types.h"

typedef enum
{
    LCE_UNKNOWN = 0,
    LCE_OTHER = -1,

    LCE_INVALID_ARGUMENT = -2,
    LCE_ALLOC_FAILED = -3,
} lc_error;

lc_void             lc_error_set    (lc_error error, const lc_char* context);
lc_error            lc_error_kind   (lc_void);
const lc_char*      lc_error_msg    (lc_void);

#endif