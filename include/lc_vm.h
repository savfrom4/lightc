#ifndef LC_VM_H
#define LC_VM_H
#include "lc_types.h"

typedef enum
{
    LCOP_NOOP,
    LCOP_ASSIGN,

    LCOP_PUSHNEW,
    LCOP_PUSHVALUE,
    LCOP_POP,

    LCOP_CALL,
} lc_vm_op;

typedef struct
{
    lc_value *stack[512];
    lc_list* types;
    lc_usize top;
} lc_vm;

lc_vm *lc_vm_new(void);
lc_void lc_vm_free(lc_vm *vm);
lc_void lc_vm_dump(lc_vm *vm);

lc_void lc_vm_push(lc_vm *vm, lc_value *value);
lc_usize lc_vm_top(lc_vm *vm);
lc_value *lc_vm_value(lc_vm *vm);
lc_void lc_vm_pop(lc_vm *vm);

#endif
