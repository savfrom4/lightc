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

#define DUMP_INT16(x)              \
    (uint8_t)(((uint16_t)x) >> 0), \
        (uint8_t)(((uint16_t)x) >> 8),

const static lc_uint8 BYTECODE[] = {
    LCOP_PUSH_NEW,
    DUMP_INT32(7),
    DUMP_INT32(69),

    LCOP_PUSH_NEW,
    DUMP_INT32(7),
    DUMP_INT32(69),

    LCOP_IADD,
    DUMP_INT32(0),
    DUMP_INT32(1),
};

int main()
{
    lc_vm *vm = lc_vm_new(BYTECODE, sizeof(BYTECODE));
    lc_vm_run(vm);

    // int32_t *value = lc_vm_to_int(vm, 0);
    // printf("i love %d\n", *value);
    // lc_vm_pop(vm, 4);

    lc_vm_dump(vm);
    lc_vm_free(vm);
    return 0;
}
