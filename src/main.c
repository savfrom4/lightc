#include "lc.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include "lc_vm.h"
#include <stdio.h>

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

const static lc_uint8 BYTECODE[] = {
    LCOP_PUSHNEW,
    DUMP_INT64(7),
    DUMP_INT32(69),

    LCOP_PUSHNEW,
    DUMP_INT64(3),
    DUMP_INT64(5),
    'H',
    'e',
    'l',
    'l',
    'o',
};

int main()
{
    lc_vm *vm = lc_vm_new(BYTECODE, sizeof(BYTECODE));
    lc_vm_run(vm);
    lc_vm_dump(vm);

    // first penis
    lc_value *value = lc_vm_value(vm);
    printf("%s, world!\n", ((lc_string *)value->data)->data);
    lc_vm_pop(vm);

    // second penis
    value = lc_vm_value(vm);
    printf("real chads love number %d uwuu~~~\n", *(lc_int32 *)value->data);
    lc_vm_pop(vm);

    lc_vm_dump(vm);
    lc_vm_free(vm);
    return 0;
}
