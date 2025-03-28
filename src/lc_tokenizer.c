#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_types.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// internal functions
lc_bool lc_token_append(lc_list *list, lc_token_type type, lc_string *value);
lc_bool lc_token_parse_fn(lc_list *list, const lc_char *ptr, lc_usize size);
lc_void lc_token_error_line(const lc_char *line_str, lc_usize line_size, lc_usize line_num, const lc_char *message, lc_usize position);

static lc_string STATIC_TOKEN_NAMES[] = {
    lc_string_comptime("none"),
    lc_string_comptime("keyword"),
    lc_string_comptime("identifier"),
    lc_string_comptime("operator"),
    lc_string_comptime("delimiter"),
    lc_string_comptime("string literal"),
    lc_string_comptime("numeric literal"),
    lc_string_comptime("semicolon"),
};

static lc_string STATIC_KEYWORDS[] = {
    lc_string_comptime("struct"),
    lc_string_comptime("if"),
    lc_string_comptime("elif"),
    lc_string_comptime("else"),
    lc_string_comptime("return"),
};
static const lc_usize STATIC_KEYWORDS_SIZE = sizeof(STATIC_KEYWORDS) / sizeof(lc_string);

lc_string *lc_token_name(lc_token_type type)
{
    return &STATIC_TOKEN_NAMES[type];
}

lc_list *lc_token_parse(const lc_string *code)
{
    lc_list *list = lc_list_new(lc_token, 64);
    lc_error_return_if(LCE_ALLOC_FAILED, NULL, !list);

    if (!lc_token_parse_fn(list, code->data, code->size))
    {
        lc_list_free(list);
        return NULL;
    }

    return list;
}

inline lc_bool lc_token_parse_fn(lc_list *list, const lc_char *ptr, lc_usize size)
{
    lc_usize i = 0, start = 0;
    lc_usize line_num = 1;

    while (i < size)
    {
        if (isalpha(ptr[i])) // identifier
        {
            start = i;
            while (i < size && (isalnum(ptr[i]) || ptr[i] == '_'))
                i++;

            lc_token_type type = LCTK_IDENTIFIER;

            for (lc_usize j = 0; j < STATIC_KEYWORDS_SIZE; j++)
            {
                const lc_string *keyword = &STATIC_KEYWORDS[j];
                if ((i - start) != keyword->size)
                    continue;

                if (strncmp(&ptr[start], keyword->data, keyword->size) == 0)
                {
                    type = LCTK_KEYWORD;
                    break;
                }
            }

            if (!lc_token_append(list, type, lc_string_new(&ptr[start], (i - start))))
                return false;

            continue;
        }
        else if (isdigit(ptr[i])) // numeric literal
        {
            start = i;
            while (i < size && (isdigit(ptr[i]) || ptr[i] == '.'))
                i++;

            if (!lc_token_append(list, LCTK_NUMERIC_LITERAL, lc_string_new(&ptr[start], (i - start))))
                return false;

            continue;
        }

        switch (ptr[i])
        {
        case ';':
            i++;
            if (!lc_token_append(list, LCTK_SEMICOLON, NULL))
                return false;
            break;

        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case ',':
            if (!lc_token_append(list, LCTK_DELIMITER, lc_string_new(&ptr[i++], sizeof(lc_char))))
                return false;
            break;

        case '!':
        case '~':
            if (!lc_token_append(list, LCTK_OPERATOR, lc_string_new(&ptr[i++], sizeof(lc_char))))
                return false;
            break;

        case '<':
        case '>':
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
            // ex. ==, >=, <=
            if (i + 1 < size && ptr[i + 1] == '=')
            {
                if (!lc_token_append(list, LCTK_OPERATOR, lc_string_new(&ptr[i], sizeof(lc_char) * 2)))
                    return false;

                i += 2;
                break;
            }

            if (!lc_token_append(list, LCTK_OPERATOR, lc_string_new(&ptr[i++], sizeof(lc_char))))
                return false;
            break;

        case '\"': // string literal
            start = ++i;
            while (i < size && ptr[i] != '\"')
            {
                if (ptr[i] == '\n')
                {
                    lc_token_error_line(ptr, size, line_num, "Expected '\"' (end of string literal), got end of the line.", i - 1);
                    return false;
                }

                i++;
            }

            if (!lc_token_append(list, LCTK_STRING_LITERAL, lc_string_new(&ptr[start], (i - start))))
                return false;

            i++;
            break;

        case '\n':
            line_num++;
            i++;
            break;

        default:
            i++;
            break;
        }
    }

    return true;
}

inline lc_bool lc_token_append(lc_list *list, lc_token_type type, lc_string *value)
{
    lc_token token = {
        .type = type,
        .value = value,
    };

    if (!lc_list_append(list, &token))
        return false;

    return true;
}

inline lc_void lc_token_error_line(const lc_char *ptr, lc_usize size, lc_usize line_num, const lc_char *message, lc_usize position)
{
    lc_usize i = 0;
    const lc_char *start = ptr;
    while (start)
    {
        start = strchr(start, '\n') + 1;
        i++;
    }

    lc_string *where = lc_string_format("%*s^ \n", position, "");
    lc_string *what = lc_string_format("%s\n %d | %.*s\n   | %s", message, line_num, size, ptr, where->data);
    lc_error_set(LCE_PARSE_ERROR, what->data);

    lc_string_free(where);
    lc_string_free(what);
}