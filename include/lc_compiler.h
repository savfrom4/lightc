#ifndef LC_COMPILER
#define LC_COMPILER
#include "lc_types.h"

typedef enum
{
    LCTK_TYPE,
    LCTK_KEYWORD,
    LCTK_IDENTIFIER,
    LCTK_OPERATOR,
    LCTK_LITERAL,
    LCTK_DELIMITER
} lc_token_type;

typedef struct
{
    lc_token_type type;
    lc_string *value;
} lc_token;

lc_list *lc_token_parse(const lc_string *code);
lc_string *lc_token_name(lc_token_type type);

#endif