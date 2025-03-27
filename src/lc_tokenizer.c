#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_types.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// internal functions
lc_bool lc_token_parse_line(lc_list *list, const lc_char *line_str, lc_usize line_size, lc_usize line_num);
lc_void lc_token_error_line(const lc_char *line_str, lc_usize line_size, lc_usize line_num, const lc_char *message, lc_usize position);

static lc_string STATIC_TOKEN_NAMES[] = {
    lc_string_comptime("none"),
    lc_string_comptime("type"),
    lc_string_comptime("keyword"),
    lc_string_comptime("identifier"),
    lc_string_comptime("operator"),
    lc_string_comptime("numeric literal"),
    lc_string_comptime("string literal"),
    lc_string_comptime("delimiter"),
};

lc_string *lc_token_name(lc_token_type type)
{
    return &STATIC_TOKEN_NAMES[type];
}

lc_list *lc_token_parse(const lc_string *code)
{
    lc_list *list = lc_list_new(lc_token, 64);
    lc_error_return_if(LCE_ALLOC_FAILED, NULL, !list);

    lc_char *line_str = code->data, *line_end = NULL;
    lc_usize line_num = 1, line_size = 0;

    do
    {
        line_end = strchr(line_str, '\n');

        if (line_end)
            line_size = (line_end - line_str);
        else if (line_num == 1)
            line_size = code->size;

        if (!lc_token_parse_line(list, line_str, line_size, line_num))
        {
            lc_list_free(list);
            return NULL;
        }

        line_str = line_end + 1;
        line_num++;
    } while (line_end);

    return list;
}

inline lc_bool lc_token_parse_line(lc_list *list, const lc_char *line_str, lc_usize line_size, lc_usize line_num)
{
    lc_token token = {
        LCTK_NONE,
        NULL,
    };

    for (lc_usize i = 0; i < line_size; i++)
    {
        lc_char c = line_str[i];
    }

    if (!lc_list_append(list, &token))
        return false;

    return true;
}

inline lc_void lc_token_error_line(const lc_char *line_str, lc_usize line_size, lc_usize line_num, const lc_char *message, lc_usize position)
{
    lc_string *where = lc_string_format("%*s^ \n", position, "");
    lc_string *what = lc_string_format("%s\n %d | %.*s\n   | %s", message, line_num, line_size, line_str, where->data);
    lc_error_set(LCE_PARSE_ERROR, what->data);

    lc_string_free(where);
    lc_string_free(what);
}