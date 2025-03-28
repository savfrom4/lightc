#ifndef LC_COMPILER
#define LC_COMPILER
#include "lc_types.h"

typedef enum
{
    LCTK_NONE,
    LCTK_KEYWORD,
    LCTK_IDENTIFIER,
    LCTK_OPERATOR,
    LCTK_DELIMITER,
    LCTK_STRING_LITERAL,
    LCTK_NUMERIC_LITERAL,

    LCTK_COUNT,
} lc_token_type;

typedef enum
{
    LCTKW_STRUCT,
    LCTKW_FOR,
    LCTKW_WHILE,
    LCTKW_IF,
    LCTKW_ELIF,
    LCTKW_ELSE,
    LCTKW_RETURN,

    LCTKW_COUNT,
} lc_token_keyword;

typedef enum
{
    LCTOP_NOT,
    LCTOP_EQ,
    LCTOP_NOTEQ,
    LCTOP_LESS,
    LCTOP_LESSEQ,
    LCTOP_GRE,
    LCTOP_GREEQ,

    LCTOP_ASSIGN,
    LCTOP_ADD,
    LCTOP_ADDEQ,
    LCTOP_SUB,
    LCTOP_SUBEQ,
    LCTOP_MUL,
    LCTOP_MULEQ,
    LCTOP_DIV,
    LCTOP_DIVEQ,

    LCTOP_COUNT,
} lc_token_operator;

typedef struct
{
    lc_token_type type;
    lc_usize pos;

    union {
        lc_token_keyword key;
        lc_token_operator op;
        lc_char delim;
        lc_string *str;
    } u;
} lc_token;

lc_list *lc_tokenizer_parse(const lc_string *code);

lc_string *lc_tokenizer_token_name(lc_token_type type);
lc_string *lc_tokenizer_keyword_name(lc_token_keyword key);
lc_string *lc_tokenizer_operator_name(lc_token_operator op);
lc_void lc_tokenizer_token_dump(const lc_token *token);

#endif