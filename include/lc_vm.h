#ifndef LC_VM_H
#define LC_VM_H
#include "lc_types.h"

typedef enum
{
    LCOP_NOOP,

    LCOP_PUSH,
    LCOP_POP,

    LCOP_IADD,
    LCOP_ISUB,
    LCOP_IMUL,
    LCOP_IDIV,

    LCOP_FADD,
    LCOP_FSUB,
    LCOP_FMUL,
    LCOP_FDIV,

    LCOP_CALL,
} lc_vm_op;

typedef struct
{
    lc_list *types;
    lc_value stack[512];
    lc_int32 top;

    lc_uint8 *btc;
    lc_usize btc_size;
    lc_usize btc_roller;
} lc_vm;

lc_vm *lc_vm_new(const lc_uint8 *bytecode, lc_usize bytecode_size);
lc_void lc_vm_free(lc_vm *vm);
lc_void lc_vm_dump(lc_vm *vm);

lc_bool lc_vm_push_value(lc_vm *vm, lc_value *value);
lc_void lc_vm_pop(lc_vm *vm, lc_int32 count);
lc_value *lc_vm_to_value(lc_vm *vm, lc_int32 index);
lc_string *lc_vm_to_string(lc_vm *vm, lc_int32 index);
lc_void *lc_vm_to_int(lc_vm *vm, lc_int32 index);
lc_void *lc_vm_to_float(lc_vm *vm, lc_int32 index);
lc_int32 lc_vm_top(lc_vm *vm);

lc_int32 lc_vm_run(lc_vm *vm);

#endif
