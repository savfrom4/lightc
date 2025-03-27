#ifndef LC_COMPILER
#define LC_COMPILER
#include "lc_types.h"

typedef enum
{
    LCTK_NONE,
    LCTK_TYPE,
    LCTK_KEYWORD,
    LCTK_IDENTIFIER,
    LCTK_OPERATOR,
    LCTK_STRING_LITERAL,
    LCTK_NUMERIC_LITERAL,
    LCTK_DELIMITER
} lc_token_type;

typedef struct
{
    lc_token_type type;
    lc_string *value;
} lc_token;

lc_string *lc_token_name(lc_token_type type);
lc_list *lc_token_parse(const lc_string *code);

#endif