#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_types.h"

// internal functions

static lc_string STATIC_TOKEN_NAMES[] = {
    lc_string_comptime("type"),
    lc_string_comptime("keyword"),
    lc_string_comptime("identifier"),
    lc_string_comptime("operator"),
    lc_string_comptime("literal"),
    lc_string_comptime("delimiter"),
};

lc_list *lc_token_parse(const lc_string *code)
{
    lc_list *list = lc_list_new(lc_token, 64);
    lc_error_return_if(LCE_ALLOC_FAILED, NULL, !list);

    lc_char *ptr = code->data;
    const lc_char *end = code->data + code->size;

    lc_token token;
    while (ptr < end)
    {
        switch (*(ptr++))
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case '~':
        case '<':
        case '>':
            token.type = LCTK_OPERATOR;
            break;
        }
    }
}

lc_string *lc_token_name(lc_token_type type)
{
    return &STATIC_TOKEN_NAMES[type];
}