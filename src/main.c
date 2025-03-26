#include "lc.h"
#include "lc_error.h"
#include "lc_mem.h"
#include "lc_types.h"
#include "lc_vm.h"
#include <stdio.h>

typedef struct
{
    lc_uint16 opcode;

    union {
        lc_uint64 param[2];
        lc_uint8 raw[16];
    } param;
} inst;

lc_uint8 *decode(const inst *insts, lc_usize size)
{
}

int main()
{

    return 0;
}
