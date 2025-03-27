#include "lc.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include "lc_vm.h"
#include <stdio.h>
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
    lc_vm *vm = lc_vm_new(BYTECODE, sizeof(BYTECODE));

    clock_t start = clock();
    for (volatile int i = 0; i < 10000000; i++)
    {
        if (lc_vm_run(vm) < 0)
        {
            printf("Error during execution: %s\n", lc_error_msg());
            return 1;
        }

        lc_vm_pop(vm, 1);
    }

    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("took %f\n", elapsed_time);

    lc_vm_dump(vm);
    lc_vm_free(vm);
    return 0;
}
