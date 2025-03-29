#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_tokenizer.h"
#include "lc_types.h"
#include <stdio.h>
#include <string.h>

const static lc_token_type PATTERN_DECLARE_VAR[] = {
    LC_TK_IDENTIFIER,
    LC_TK_IDENTIFIER,
    LC_TK_NONE,
};

// internal functions
static inline lc_bool lc_compiler_eval_fn(const lc_list *tokens, lc_compiler_bytecode *bytecode);
static inline lc_bool lc_compiler_match_pattern(const lc_list *tokens, const lc_token_type *pattern, lc_usize start, lc_usize pattern_size);

lc_compiler_bytecode *lc_compiler_eval(const lc_list *tokens)
{
    lc_compiler_bytecode *bytecode;
    lc_error_return_if(LC_E_ALLOC_FAILED, NULL, !(bytecode = lc_mem_alloc(sizeof(lc_compiler_bytecode))));

    // allocate bytecode
    if (!(bytecode->btc = lc_list_new(lc_uint8, 64)))
    {
        lc_mem_free(bytecode);
        return NULL;
    }

    if (!lc_compiler_eval_fn(tokens, bytecode))
    {
        lc_compiler_free(bytecode);
        return NULL;
    }

    return bytecode;
}

lc_void lc_compiler_free(lc_compiler_bytecode *compiled)
{
    lc_list_free(compiled->btc);
    lc_mem_free(compiled);
}

static inline lc_bool lc_compiler_eval_fn(const lc_list *tokens, lc_compiler_bytecode *compiled)
{
    // variable declaration
    if (lc_compiler_match_pattern(tokens, PATTERN_DECLARE_VAR, 0, sizeof(PATTERN_DECLARE_VAR) / sizeof(lc_token_type)))
    {
    }

    return true;
}

static inline lc_bool lc_compiler_match_pattern(const lc_list *tokens, const lc_token_type *pattern, lc_usize start, lc_usize pattern_size)
{
    lc_usize i = 0, n = 0;
    lc_list_foreach(tokens, it)
    {
        if (i++ < start)
            continue;

        lc_token *token = (lc_token *)it;
        if (pattern[n] != LC_TK_NONE)
        {
            if (token->type != pattern[n])
                return false;
        }

        n++;
        if (n >= pattern_size)
            return true;
    }

    return false;
}