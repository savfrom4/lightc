#include "lc_error.h"
#include "lc_types.h"
#include <stdio.h>

static lc_error g_error = LCE_UNKNOWN;
static lc_char g_error_msg[256];

lc_void lc_error_set(lc_error error, const lc_char *context)
{
    g_error = error;
    switch (error)
    {
    case LCE_UNKNOWN:
        snprintf(g_error_msg, sizeof(g_error_msg), "Unknown error.");
        break;

    case LCE_OTHER:
        snprintf(g_error_msg, sizeof(g_error_msg), "%s", context);
        break;

    case LCE_INVALID_ARGUMENT:
        snprintf(g_error_msg, sizeof(g_error_msg), "Invalid argument: %s", context);
        break;

    case LCE_ALLOC_FAILED:
        snprintf(g_error_msg, sizeof(g_error_msg), "Allocation failed for: %s", context);
        break;
    }
}

lc_error lc_error_kind(lc_void)
{
    return g_error;
}

const lc_char *lc_error_msg(lc_void)
{
    return g_error_msg;
}