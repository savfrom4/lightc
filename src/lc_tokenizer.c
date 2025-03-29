#include "lc_tokenizer.h"
#include "lc_error.h"
#include "lc_types.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static lc_string STATIC_TOKEN_NAMES[LC_TK_COUNT] = {
    lc_string_comptime("none"),
    lc_string_comptime("keyword"),
    lc_string_comptime("identifier"),
    lc_string_comptime("operator"),
    lc_string_comptime("delimiter"),
    lc_string_comptime("string literal"),
    lc_string_comptime("numeric literal"),
};

static lc_string STATIC_KEYWORDS[LC_TKW_COUNT] = {
    lc_string_comptime("struct"),
    lc_string_comptime("for"),
    lc_string_comptime("while"),
    lc_string_comptime("if"),
    lc_string_comptime("elif"),
    lc_string_comptime("else"),
    lc_string_comptime("return"),
};

static lc_string STATIC_OPERATORS[LC_TKOP_COUNT] = {
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
static inline lc_bool lc_tokenizer_append(lc_list *list, lc_token token);
static inline lc_bool lc_tokenizer_append_operator(lc_list *list, const lc_string *code, lc_usize *i, lc_token_operator a, lc_token_operator b);
static inline lc_bool lc_tokenizer_parse_fn(lc_list *list, const lc_string *code, lc_usize i);
static inline lc_void lc_tokenizer_error_line(const lc_string *code, lc_usize position, const lc_char *message);

lc_list *lc_tokenizer_parse(const lc_string *code)
{
    lc_list *list = lc_list_new(lc_token, 64);
    lc_error_return_if(LC_E_ALLOC_FAILED, NULL, !list);

    if (!lc_tokenizer_parse_fn(list, code, 0))
    {
        lc_list_free(list);
        return NULL;
    }

    return list;
}

lc_string *lc_tokenizer_token_name(lc_token_type type)
{
    if (type >= LC_TK_COUNT)
        return NULL;

    return &STATIC_TOKEN_NAMES[type];
}

lc_string *lc_tokenizer_keyword_name(lc_token_keyword type)
{
    if (type >= LC_TKW_COUNT)
        return NULL;

    return &STATIC_KEYWORDS[type];
}

lc_string *lc_tokenizer_operator_name(lc_token_operator type)
{
    if (type >= LC_TKOP_COUNT)
        return NULL;

    return &STATIC_OPERATORS[type];
}

lc_void lc_tokenizer_token_dump(const lc_token *token)
{
    switch (token->type)
    {
    case LC_TK_NONE:
        printf("lc_token {}\n");
        break;

    case LC_TK_KEYWORD:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, STATIC_KEYWORDS[token->u.key].data);
        break;

    case LC_TK_OPERATOR:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, STATIC_OPERATORS[token->u.op].data);
        break;

    case LC_TK_IDENTIFIER:
    case LC_TK_STRING_LITERAL:
    case LC_TK_NUMERIC_LITERAL:
        printf("lc_token { %s, \"%s\" }\n", STATIC_TOKEN_NAMES[token->type].data, token->u.str->data);
        break;

    case LC_TK_DELIMITER:
        printf("lc_token { %s, '%c' }\n", STATIC_TOKEN_NAMES[token->type].data, token->u.delim);
        break;

    default:
        return;
    }
}

static inline lc_bool lc_tokenizer_parse_fn(lc_list *list, const lc_string *code, lc_usize i)
{
    lc_usize start = 0;

    while (i < code->size)
    {
        lc_char c = code->data[i];

        if (isalpha(c) || c == '_') // identifier or keyword
        {
            start = i;

            while (i < code->size && (isalnum(code->data[i]) || code->data[i] == '_'))
                i++;

            // check if its keyword
            for (lc_usize j = 0; j < LC_TKW_COUNT; j++)
            {
                const lc_string *keyword = &STATIC_KEYWORDS[j];
                if ((i - start) != keyword->size)
                    continue;

                if (strncmp(&code->data[start], keyword->data, keyword->size) == 0)
                {
                    if (!lc_tokenizer_append(list, (lc_token){LC_TK_KEYWORD, i, {.key = (lc_token_keyword)j}}))
                        return false;
                }
            }

            if (!lc_tokenizer_append(list, (lc_token){LC_TK_IDENTIFIER, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;
            continue;
        }
        else if (isdigit(c)) // numeric literal
        {
            start = i;
            while (i < code->size && (isdigit(code->data[i]) || code->data[i] == '.'))
                i++;

            if (!lc_tokenizer_append(list, (lc_token){LC_TK_NUMERIC_LITERAL, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;

            continue;
        }
        else if (isspace(c))
        {
            i++;
            continue;
        }

        switch (c)
        {
        case ';':
        case ',':
            if (!lc_tokenizer_append(list, (lc_token){LC_TK_DELIMITER, i, {.delim = c}}))
                return false;

            i++;
            break;

        /* braces */
        case '(':
        case '[':
        case '{':
            if (!lc_tokenizer_append(list, (lc_token){LC_TK_DELIMITER, i, {.delim = c}}))
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

            if (!lc_tokenizer_append(list, (lc_token){LC_TK_DELIMITER, i, {.delim = delim}}))
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

        /* string literal */
        case '\"': {
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

            if (!lc_tokenizer_append(list, (lc_token){LC_TK_STRING_LITERAL, i, {.str = lc_string_new(&code->data[start], (i - start))}}))
                return false;

            i++;
            break;
        }

        /* operators */
        case '!':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_NOT, LC_TKOP_NOTEQ))
                return false;
            break;

        case '<':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_LESS, LC_TKOP_LESSEQ))
                return false;
            break;

        case '>':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_GRE, LC_TKOP_GREEQ))
                return false;
            break;

        case '+':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_ADD, LC_TKOP_ADDEQ))
                return false;
            break;

        case '-':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_SUB, LC_TLOP_SUBEQ))
                return false;
            break;

        case '*':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_MUL, LC_TKOP_MULEQ))
                return false;
            break;

        case '/':
            // this can also be a comment
            if (i + 1 < code->size && code->data[i + 1] == '/')
            {
                i++;

                while (code->data[i] != '\"')
                {
                    if (code->data[i] == '\n' || i >= code->size)
                        break;

                    i++;
                }

                break;
            }

            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_DIV, LC_TKOP_DIVEQ))
                return false;
            break;

        case '=':
            if (!lc_tokenizer_append_operator(list, code, &i, LC_TKOP_ASSIGN, LC_TKOP_EQ))
                return false;
            break;

        case '.':
            if (!lc_tokenizer_append(list, (lc_token){LC_TK_OPERATOR, i++, {.op = LC_TKOP_ACCESS}}))
                return false;
            break;

        default:
            lc_tokenizer_error_line(code, i, "Unsupported character encountered.");
            return false;
        }
    }

    return true;
}

static inline lc_bool lc_tokenizer_append(lc_list *list, lc_token token)
{
    if (!lc_list_append(list, &token))
        return false;

    return true;
}

static inline lc_bool lc_tokenizer_append_operator(lc_list *list, const lc_string *code, lc_usize *i, lc_token_operator a, lc_token_operator b)
{
    // <=, ==, !=, ....
    if (*i + 1 < code->size && code->data[*i + 1] == '=')
    {
        if (!lc_tokenizer_append(list, (lc_token){LC_TK_OPERATOR, *i, {.op = b}}))
            return false;

        (*i) += 2;
        return true;
    }

    if (!lc_tokenizer_append(list, (lc_token){LC_TK_OPERATOR, *i, {.op = a}}))
        return false;

    (*i)++;
    return true;
}

static lc_void lc_tokenizer_error_line(const lc_string *code, lc_usize position, const lc_char *message)
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
    lc_error_set(LC_E_PARSE_ERROR, what->data);

    lc_string_free(where);
    lc_string_free(what);
}