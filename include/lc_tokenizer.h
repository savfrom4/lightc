#ifndef LC_COMPILER
#define LC_COMPILER
#include "lc_types.h"

typedef enum
{
    LC_TK_NONE,
    LC_TK_KEYWORD,
    LC_TK_IDENTIFIER,
    LC_TK_OPERATOR,
    LC_TK_DELIMITER,
    LC_TK_STRING_LITERAL,
    LC_TK_NUMERIC_LITERAL,

    LC_TK_COUNT,
} lc_token_type;

typedef enum
{
    LC_TKW_STRUCT,
    LC_TKW_FOR,
    LC_TKW_WHILE,
    LC_TKW_IF,
    LC_TKW_ELIF,
    LC_TKW_ELSE,
    LC_TKW_RETURN,

    LC_TKW_COUNT,
} lc_token_keyword;

typedef enum
{
    LC_TKOP_NOT,
    LC_TKOP_EQ,
    LC_TKOP_NOTEQ,
    LC_TKOP_LESS,
    LC_TKOP_LESSEQ,
    LC_TKOP_GRE,
    LC_TKOP_GREEQ,

    LC_TKOP_ASSIGN,
    LC_TKOP_ACCESS,
    LC_TKOP_ADD,
    LC_TKOP_ADDEQ,
    LC_TKOP_SUB,
    LC_TKOP_SUBEQ,
    LC_TKOP_MUL,
    LC_TKOP_MULEQ,
    LC_TKOP_DIV,
    LC_TKOP_DIVEQ,

    LC_TKOP_COUNT,
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