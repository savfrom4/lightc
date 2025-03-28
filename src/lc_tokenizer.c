#include "lc_tokenizer.h"
#include "lc_error.h"
#include "lc_types.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static lc_string STATIC_TOKEN_NAMES[LCTK_COUNT] = {
    lc_string_comptime("none"),
    lc_string_comptime("keyword"),
    lc_string_comptime("identifier"),
    lc_string_comptime("operator"),
    lc_string_comptime("delimiter"),
    lc_string_comptime("string literal"),
    lc_string_comptime("numeric literal"),
};

static lc_string STATIC_KEYWORDS[LCTKW_COUNT] = {
    lc_string_comptime("struct"),
    lc_string_comptime("for"),
    lc_string_comptime("while"),
    lc_string_comptime("if"),
    lc_string_comptime("elif"),
    lc_string_comptime("else"),
    lc_string_comptime("return"),
};

static lc_string STATIC_OPERATORS[LCTOP_COUNT] = {
    lc_string_comptime("!"),
    lc_string_comptime("=="),
    lc_string_comptime("!="),
    lc_string_comptime("<"),
    lc_string_comptime("<="),
    lc_string_comptime(">"),
    lc_string_comptime(">="),
    lc_string_comptime("="),
    lc_string_comptime("+"),
    lc_string_comptime("+="),
    lc_string_comptime("-"),
    lc_string_comptime("-="),
    lc_string_comptime("*"),
    lc_string_comptime("*="),
    lc_string_comptime("/"),
    lc_string_comptime("/="),
};

// internal functions
lc_bool lc_tokenizer_append(lc_list *list, lc_token token);
lc_bool lc_tokenizer_append_operator(lc_list *list, const lc_string *code, lc_usize *i, lc_token_operator a, lc_token_operator b);
lc_bool lc_tokenizer_parse_fn(lc_list *list, const lc_string *code, lc_usize i);
lc_void lc_tokenizer_error_line(const lc_string *code, lc_usize position, const lc_char *message);

lc_list *lc_tokenizer_parse(const lc_string *code)
{
    lc_list *list = lc_list_new(lc_token, 64);
    lc_error_return_if(LCE_ALLOC_FAILED, NULL, !list);

    if (!lc_tokenizer_parse_fn(list, code, 0))
    {
        lc_list_free(list);
        return NULL;
    }

    return list;
}

lc_string *lc_tokenizer_token_name(lc_token_type type)
{
    if (type >= LCTK_COUNT)
        return NULL;

    return &STATIC_TOKEN_NAMES[type];
}

lc_string *lc_tokenizer_keyword_name(lc_token_keyword type)
{
    if (type >= LCTKW_COUNT)
        return NULL;

    return &STATIC_KEYWORDS[type];
}

lc_string *lc_tokenizer_operator_name(lc_token_operator type)
{
    if (type >= LCTOP_COUNT)
        return NULL;

    return &STATIC_OPERATORS[type];
}

lc_void lc_tokenizer_token_dump(const lc_token *token)
{
    switch (token->type)
    {
    case LCTK_NONE:
        printf("lc_token {}\n");
        break;

    case LCTK_KEYWORD:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, STATIC_KEYWORDS[token->u.key].data);
        break;

    case LCTK_OPERATOR:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, STATIC_OPERATORS[token->u.op].data);
        break;

    case LCTK_IDENTIFIER:
    case LCTK_STRING_LITERAL:
    case LCTK_NUMERIC_LITERAL:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, token->u.str->data);
        break;

    case LCTK_DELIMITER:
        printf("lc_token { %s, '%c' }\n", STATIC_TOKEN_NAMES[token->type].data, token->u.delim);
        break;

    default:
        return;
    }
}

inline lc_bool lc_tokenizer_parse_fn(lc_list *list, const lc_string *code, lc_usize i)
{
    lc_usize start = 0;

    while (i < code->size)
    {
        if (isalpha(code->data[i])) // identifier
        {
            start = i;

            while (i < code->size && (isalnum(code->data[i]) || code->data[i] == '_'))
                i++;

            for (lc_usize j = 0; j < LCTKW_COUNT; j++)
            {
                const lc_string *keyword = &STATIC_KEYWORDS[j];
                if ((i - start) != keyword->size)
                    continue;

                if (strncmp(&code->data[start], keyword->data, keyword->size) == 0)
                {
                    if (!lc_tokenizer_append(list, (lc_token){LCTK_KEYWORD, i, {.key = (lc_token_keyword)j}}))
                        return false;
                }
            }

            if (!lc_tokenizer_append(list, (lc_token){LCTK_IDENTIFIER, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;
            continue;
        }
        else if (isdigit(code->data[i])) // numeric literal
        {
            start = i;
            while (i < code->size && (isdigit(code->data[i]) || code->data[i] == '.'))
                i++;

            if (!lc_tokenizer_append(list, (lc_token){LCTK_NUMERIC_LITERAL, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;

            continue;
        }

        lc_char c = code->data[i];
        switch (c)
        {
        case ';':
        case ',':
            if (!lc_tokenizer_append(list, (lc_token){LCTK_DELIMITER, i, {.delim = c}}))
                return false;

            i++;
            break;

        case '(':
        case '[':
        case '{':
            if (!lc_tokenizer_append(list, (lc_token){LCTK_DELIMITER, i, {.delim = c}}))
                return false;

            start = ++i;

            lc_char delim;
            lc_char *delim_name;
            switch (c)
            {
            case '(':
                delim = ')';
                delim_name = "closing parenthesis";
                break;

            case '[':
                delim = ']';
                delim_name = "closing bracket";
                break;

            case '{':
                delim = '}';
                delim_name = "closing brace";
                break;
            }

            lc_usize cnt = 0, end = 0;
            while (i < code->size)
            {
                if (code->data[i] == delim)
                {
                    if (cnt == 0)
                        end = i;
                    else
                        cnt--;
                }
                else if (code->data[i] == c)
                    cnt++;

                i++;
            }

            if (!end)
            {
                char error[512];
                snprintf(error, 512, "Expected '%c' (%s).", delim, delim_name);

                lc_tokenizer_error_line(code, start - 1, error);
                return false;
            }

            if (!lc_tokenizer_parse_fn(list, &(lc_string){code->data, end}, start))
                return false;

            if (!lc_tokenizer_append(list, (lc_token){LCTK_DELIMITER, i, {.delim = delim}}))
                return false;

            i = end + 1;
            break;

        case ')':
            lc_tokenizer_error_line(code, i, "Expected '(' (opening parenthesis).");
            return false;

        case ']':
            lc_tokenizer_error_line(code, i, "Expected '[' (opening brace).");
            return false;

        case '}':
            lc_tokenizer_error_line(code, i, "Expected '{' (opening parenthesis).");
            return false;

        case '!':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_NOT, LCTOP_NOTEQ))
                return false;
            break;

        case '<':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_LESS, LCTOP_LESSEQ))
                return false;
            break;

        case '>':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_GRE, LCTOP_GREEQ))
                return false;
            break;

        case '+':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_ADD, LCTOP_ADDEQ))
                return false;
            break;

        case '-':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_SUB, LCTOP_SUBEQ))
                return false;
            break;

        case '*':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_MUL, LCTOP_MULEQ))
                return false;
            break;

        case '/':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_DIV, LCTOP_DIVEQ))
                return false;
            break;

        case '=':
            if (!lc_tokenizer_append_operator(list, code, &i, LCTOP_ASSIGN, LCTOP_EQ))
                return false;
            break;

        case '\"': // string literal
            start = ++i;

            while (code->data[i] != '\"')
            {
                if (code->data[i] == '\n' || i >= code->size)
                {
                    lc_tokenizer_error_line(code, i - 1, "Expected '\"' (end of string literal), got end of the line.");
                    return false;
                }

                i++;
            }

            if (!lc_tokenizer_append(list, (lc_token){LCTK_STRING_LITERAL, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;

            i++;
            break;

        default:
            i++;
            break;
        }
    }

    return true;
}

inline lc_bool lc_tokenizer_append(lc_list *list, lc_token token)
{
    if (!lc_list_append(list, &token))
        return false;

    return true;
}

inline lc_bool lc_tokenizer_append_operator(lc_list *list, const lc_string *code, lc_usize *i, lc_token_operator a, lc_token_operator b)
{
    // <=, ==, !=, ....
    if (*i + 1 < code->size && code->data[*i + 1] == '=')
    {
        if (!lc_tokenizer_append(list, (lc_token){LCTK_OPERATOR, *i, {.op = b}}))
            return false;

        (*i) += 2;
        return true;
    }

    if (!lc_tokenizer_append(list, (lc_token){LCTK_OPERATOR, *i, {.op = a}}))
        return false;

    (*i)++;
    return true;
}

lc_void lc_tokenizer_error_line(const lc_string *code, lc_usize position, const lc_char *message)
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

            line_size = i - line_start;
            break;
        }
    }

    lc_string *where = lc_string_format("%*s^ \n", position - line_start, "");
    lc_string *what = lc_string_format("%s\n %d | %.*s\n   | %s", message, line_num, line_size, &code->data[line_start], where->data);
    lc_error_set(LCE_PARSE_ERROR, what->data);

    lc_string_free(where);
    lc_string_free(what);
}