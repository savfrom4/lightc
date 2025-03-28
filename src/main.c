#include "lc.h"
#include "lc_compiler.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include "lc_vm.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DUMP_INT64(x)                   \
    (uint8_t)(((uint64_t)x) >> 0),      \
        (uint8_t)(((uint64_t)x) >> 8),  \
        (uint8_t)(((uint64_t)x) >> 16), \
        (uint8_t)(((uint64_t)x) >> 24), \
        (uint8_t)(((uint64_t)x) >> 32), \
        (uint8_t)(((uint64_t)x) >> 40), \
        (uint8_t)(((uint64_t)x) >> 48), \
        (uint8_t)(((uint64_t)x) >> 56)

#define DUMP_INT32(x)                   \
    (uint8_t)(((uint32_t)x) >> 0),      \
        (uint8_t)(((uint32_t)x) >> 8),  \
        (uint8_t)(((uint32_t)x) >> 16), \
        (uint8_t)(((uint32_t)x) >> 24)

#define DUMP_INT16(x)              \
    (uint8_t)(((uint16_t)x) >> 0), \
        (uint8_t)(((uint16_t)x) >> 8)

static const lc_uint8 BYTECODE[] = {
    LCOP_PUSH,
    LCT_INT16,
    DUMP_INT16(69),

    LCOP_PUSH,
    LCT_INT32,
    DUMP_INT32(UINT16_MAX + 1),

    LCOP_IADD,
};

int main()
{
    char buffer[4096] = {};
    FILE *file = fopen("../tests/test1.lc", "r");
    if (!file)
        return 1;

    fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    printf("%s\n\n", buffer);

    lc_list *tokens = lc_token_parse(&(lc_string){buffer, strlen(buffer)});
    if (!tokens)
    {
        printf("Error: %s\n", lc_error_msg());
        return 1;
    }

    lc_list_foreach(tokens, it)
    {
        lc_token *token = it;
        printf("%s ", token->data ? token->data->data : "null");
    }

    return 0;
}
