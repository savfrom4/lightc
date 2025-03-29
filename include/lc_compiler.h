#ifndef LC_COMPILER_H
#define LC_COMPILER_H
#include "lc_types.h"

typedef struct
{
    lc_list *btc;
} lc_compiler_bytecode;

lc_compiler_bytecode *lc_compiler_eval(const lc_list *tokens);
lc_void lc_compiler_free(lc_compiler_bytecode *compiled);

#endif