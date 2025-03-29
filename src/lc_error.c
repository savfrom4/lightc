#include "lc_error.h"
#include "lc_types.h"
#include <stdio.h>

static lc_error g_error = LC_E_UNKNOWN;
static lc_char g_error_msg[1024] = {};

lc_void lc_error_set(lc_error error, const lc_char *context)
{
    g_error = error;
    switch (error)
    {
    case LC_E_UNKNOWN:
        snprintf(g_error_msg, sizeof(g_error_msg), "Unknown error.");
        break;

    case LC_E_OTHER:
        snprintf(g_error_msg, sizeof(g_error_msg), "%s", context);
        break;

    case LC_E_INVALID_ARGUMENT:
        snprintf(g_error_msg, sizeof(g_error_msg), "Invalid argument: %s", context);
        break;

    case LC_E_ALLOC_FAILED:
        snprintf(g_error_msg, sizeof(g_error_msg), "Allocation failed for: %s", context);
        break;

    case LC_E_VM_STACK_OVERFLOW:
        snprintf(g_error_msg, sizeof(g_error_msg), "Stack overflow");
        break;

    case LC_E_VM_EARLY_EOF:
        snprintf(g_error_msg, sizeof(g_error_msg), "Early end-of-file: %s", context);
        break;

    case LC_E_PARSE_ERROR:
        snprintf(g_error_msg, sizeof(g_error_msg), "Parse error: %s", context);
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