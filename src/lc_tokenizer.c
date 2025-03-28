#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_types.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// internal functions
lc_bool lc_token_append(lc_list *list, lc_token token);
lc_bool lc_token_parse_fn(lc_list *list, const lc_string *code);
lc_void lc_token_error_line(const lc_string *code, lc_usize position, const lc_char *message);

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

    if (!lc_token_parse_fn(list, code))
    {
        lc_list_free(list);
        return NULL;
    }

    return list;
}

inline lc_bool lc_token_parse_fn(lc_list *list, const lc_string *code)
{
    lc_usize i = 0, start = 0;
    lc_usize line_no = 1;

    while (i < code->size)
    {
        if (isalpha(code->data[i])) // identifier
        {
            lc_token_type type = LCTK_IDENTIFIER;
            start = i;

            while (i < code->size && (isalnum(code->data[i]) || code->data[i] == '_'))
                i++;

            for (lc_usize j = 0; j < STATIC_KEYWORDS_SIZE; j++)
            {
                const lc_string *keyword = &STATIC_KEYWORDS[j];
                if ((i - start) != keyword->size)
                    continue;

                if (strncmp(&code->data[start], keyword->data, keyword->size) == 0)
                {
                    type = LCTK_KEYWORD;
                    break;
                }
            }

            if (!lc_token_append(list, (lc_token){type, i, lc_string_new(&code->data[start], (i - start))}))
                return false;

            continue;
        }
        else if (isdigit(code->data[i])) // numeric literal
        {
            start = i;
            while (i < code->size && (isdigit(code->data[i]) || code->data[i] == '.'))
                i++;

            if (!lc_token_append(list, (lc_token){LCTK_NUMERIC_LITERAL, i, lc_string_new(&code->data[start], (i - start))}))
                return false;

            continue;
        }

        switch (code->data[i])
        {
        case ';':
            i++;
            if (!lc_token_append(list, (lc_token){LCTK_SEMICOLON, i, NULL}))
                return false;
            break;

        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case ',':
            if (!lc_token_append(list, (lc_token){LCTK_DELIMITER, i, lc_string_new(&code->data[i], sizeof(lc_char))}))
                return false;

            i++;
            break;

        case '!':
        case '~':
            if (!lc_token_append(list, (lc_token){LCTK_OPERATOR, i, lc_string_new(&code->data[i], sizeof(lc_char))}))
                return false;

            i++;
            break;

        case '<':
        case '>':
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
            // ex. ==, >=, <=
            if (i + 1 < code->size && code->data[i + 1] == '=')
            {
                if (!lc_token_append(list, (lc_token){LCTK_OPERATOR, i, lc_string_new(&code->data[i], 2)}))
                    return false;

                i += 2;
                break;
            }

            if (!lc_token_append(list, (lc_token){LCTK_OPERATOR, i, lc_string_new(&code->data[i], 1)}))
                return false;

            i++;
            break;

        case '\"': // string literal
            start = ++i;

            while (code->data[i] != '\"')
            {
                if (code->data[i] == '\n' || i >= code->size)
                {
                    lc_token_error_line(code, i - 1, "Expected '\"' (end of string literal), got end of the line.");
                    return false;
                }

                i++;
            }

            if (!lc_token_append(list, (lc_token){LCTK_STRING_LITERAL, i, lc_string_new(&code->data[start], (i - start))}))
                return false;

            i++;
            break;

        case '\n':
            line_no++;
            i++;
            break;

        default:
            i++;
            break;
        }
    }

    return true;
}

inline lc_bool lc_token_append(lc_list *list, lc_token token)
{
    if (!lc_list_append(list, &token))
        return false;

    return true;
}

lc_void lc_token_error_line(const lc_string *code, lc_usize position, const lc_char *message)
{
    // find the line
    lc_usize line_start = 0, line_size = 0, line_num = 1;
    for (lc_usize i = 0; i < code->size; i++)
    {
        if (code->data[i] == '\n')
        {
            line_start = i + 1;
            line_num++;
        }

        if (i >= position)
        {
            while (i < code->size && code->data[i] != '\n')
                i++;

            line_size = i - line_size;
            break;
        }
    }

    lc_string *where = lc_string_format("%*s^ \n", position - line_start, "");
    lc_string *what = lc_string_format("%s\n %d | %.*s\n   | %s", message, line_num, line_size, &code->data[line_start], where->data);
    lc_error_set(LCE_PARSE_ERROR, what->data);

    lc_string_free(where);
    lc_string_free(what);
}